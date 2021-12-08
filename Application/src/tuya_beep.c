/**
* @file tuya_beep.c
* @author zzw
* @email: huanling.zhang@tuya.com
* @brief tuya_beep module is used to control the buzzer sound
* @version 0.1
* @date 2021-10-12
*
* @copyright Copyright (c) tuya.inc 2021
*
*/

/*********************************************************************
********************************includes******************************
*********************************************************************/
#include "ty_pwm.h"
#include "nrf_gpio.h"
#include "tuya_ble_log.h"
/*********************************************************************
****************************private includes**************************
*********************************************************************/
#include "tuya_beep.h"
/*********************************************************************
******************************macro define****************************
*********************************************************************/
#define     KEY_OPEN        19
/*********************************************************************
****************************typedef define****************************
*********************************************************************/


/*********************************************************************
****************************variable define***************************
*********************************************************************/
static ty_pwm_t beep_pwm = {0};
static volatile uint8_t amarm_flag = 0;
static tuya_ble_timer_t alarm_timer;
/*********************************************************************
****************************function define***************************
*********************************************************************/
/**
* @function:tuya_beep_init
* @brief: buzzer port init
* @param[in]: pin_num: buzzer pin port
* @return: none
*/
void tuya_beep_init(uint8_t pin_num)
{
    beep_pwm.pin = pin_num;
    beep_pwm.polarity = 0;
    beep_pwm.freq = 100;
    beep_pwm.duty = 50;

    ty_pwm_init(&beep_pwm);
    ty_pwm_stop(&beep_pwm);

    return ;
}
void tuya_remind_miss_alarm_cb()
{
	static int count = 0;
    tuya_ble_timer_t miss_alarm_timer;

	ty_pwm_stop(&beep_pwm);
	tuya_ble_device_delay_ms(100);
	ty_pwm_start(&beep_pwm);
	count++;
    if (0 == nrf_gpio_pin_read(KEY_OPEN)) {
		ty_pwm_stop(&beep_pwm);
		tuya_ble_timer_delete(alarm_timer);
        
	}
    if (200 == count) {
        tuya_ble_timer_create(&miss_alarm_timer, 1000*60*3, TUYA_BLE_TIMER_REPEATED, tuya_remind_miss_alarm_cb);
        tuya_ble_timer_start(miss_alarm_timer);
    }

}
/**
* @function:tuya_beep_medicine_alarm
* @brief: start pwm
* @param[in]: scale_freq -> set  frequency, unit:Hz  
* @param[in]: duty -> set pwm duty cycle (0 ~ 100)
* @return: none
*/
int tuya_beep_medicine_alarm(uint32_t scale_freq, uint8_t duty)
{
    if (0 == scale_freq || duty > 100) {
        TUYA_APP_LOG_ERROR("param_in error!!!");
        return -1;
    }
    int i = 0, j = 0;
	
    beep_pwm.duty = duty;
    beep_pwm.freq = scale_freq;
	tuya_ble_timer_create(&alarm_timer, 200, TUYA_BLE_TIMER_REPEATED, tuya_remind_miss_alarm_cb);
	tuya_ble_timer_start(alarm_timer);

    return 0;
}
/**
* @function:tuya_beep_medicine_alarm_start
* @brief: 
* @param[in]: none
* @return: none
*/
void tuya_beep_medicine_alarm_start(void)
{
    amarm_flag = 1;
}
/**
* @function:tuya_beep_box_find_play
* @brief: start pwm
* @param[in]: scale_freq -> set  frequency, unit:Hz  
* @param[in]: duty -> set pwm duty cycle (0 ~ 100)
* @return: -1 -> fail
            0 -> success
*/
int tuya_beep_box_find_play(uint32_t scale_freq, uint8_t duty)
{
    if (0 == scale_freq || duty > 100) {
        TUYA_APP_LOG_ERROR("param_in error!!!");

        return -1;
    }

    beep_pwm.freq = scale_freq;
    beep_pwm.duty = duty;

    ty_pwm_start(&beep_pwm);

    return 0;
}
/**
* @function:tuya_beep_stop_play
* @brief: stop pwm
* @param[in]: none
* @return: none
*/
void tuya_beep_stop_play(void)
{
    ty_pwm_stop(&beep_pwm);

    return ;
}
/**
* @function:tuya_beep_uninit
* @brief: cancel pwm
* @param[in]: none
* @return: none
*/
void tuya_beep_uninit(void)
{
    ty_pwm_uninit(&beep_pwm);

    return ;
}
