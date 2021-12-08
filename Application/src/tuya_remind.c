/**
* @file tuya_remind.c
* @author zzw
* @email: huanling.zhang@tuya.com
* @brief tuya_remind module is used to reminders related functions
* @version 0.1
* @date 2021-10-14
*
* @copyright Copyright (c) tuya.inc 2021
*
*/

/*********************************************************************
********************************includes******************************
*********************************************************************/
#include "tuya_ble_api.h"
#include "tuya_ble_unix_time.h"
#include "tuya_battery_monitor.h"
#include "tuya_ble_port_nRF52832.h"
/*********************************************************************
****************************private includes**************************
*********************************************************************/
#include "tuya_beep.h"
#include "tuya_remind.h"
#include "tuya_dp_process.h"
/*********************************************************************
******************************macro define****************************
*********************************************************************/
#define     DP_BUF_LEN(buf) (sizeof(buf) / sizeof(char)) 
/*********************************************************************
****************************typedef define****************************
*********************************************************************/
typedef struct {    // bit fields
    uint8_t find        :1;
    uint8_t sleep       :1;
    uint8_t key_scan    :1;
    uint8_t             :5;
} TY_FLAG_INFO_T;
/*********************************************************************
****************************variable define***************************
*********************************************************************/
volatile TY_FLAG_INFO_T flag_handle;
tuya_ble_timer_t ble_conn_timer;
//static tuya_ble_timer_t miss_alarm_timer;
static uint8_t find_buf[] = {DP_BOX_FIND, BOOL, 0x00, 0x01, 0x00};
/* adc channel config(select) */
static nrf_saadc_channel_config_t key_adc = NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_AIN2);
/*********************************************************************
****************************function define***************************
*********************************************************************/
static void tuya_remind_key_cb(nrf_drv_saadc_evt_t const *p_event)
{
    return ;
}
/**
* @function:tuya_remind_medicine_key_init
* @brief: adc key init
* @param[in]: channel -> adc channel
* @return: none
*/
static void tuya_remind_medicine_key_init(uint8_t channel)
{
    nrf_drv_saadc_init(NULL, tuya_remind_key_cb);
    nrf_drv_saadc_channel_init(channel, &key_adc);

    return ;
}
/**
* @function:tuya_remind_box_init
* @brief: medicine box init
* @param[in]: none
* @return: none
*/
void tuya_remind_box_init(void)
{
	/* OLED init */
	nrf_gpio_cfg_output(KEY_OLED);
    nrf_gpio_pin_write(KEY_OLED, 1);
	tuya_ble_device_delay_ms(20);
    ty_i2c_init();
    ty_i2c_start();
    uint32_t ty_i2c_scan_address(void);
    if (0 == ty_i2c_scan_address() ) {  // slave address 0x3c	
        ty_oled_init(); // OLED resolution: 64*128
    }
    /* beep_init */
    tuya_beep_init(BUZZER_PIN);
    /* battery check init */
    tuya_batmon_batval_init(ADC_CHANNEL0);
    /* LED init */
    nrf_gpio_cfg_output(LED_1);
    nrf_gpio_cfg_output(LED_2);
    nrf_gpio_cfg_output(LED_3);
    nrf_gpio_cfg_output(LED_SWITCH);
    /* key init */
    nrf_gpio_cfg_input(KEY_OPEN, NRF_GPIO_PIN_NOPULL);  // pressed: high  unpressed: low
    /* medicine key init */
    tuya_remind_medicine_key_init(ADC_CHANNEL2);

    return ;
}
/**
* @function:tuya_remind_box_exit
* @brief: according to port function uninit the box port
* @param[in]: none
* @return: none
*/
void tuya_remind_box_exit(void)
{
    nrf_gpio_cfg_default(BUZZER_PIN);
    nrf_gpio_cfg_default(LED_SWITCH);
    nrf_gpio_cfg_default(KEY_OPEN);
    nrf_gpio_cfg_default(KEY_OLED);
    nrf_gpio_cfg_default(LED_1);
    nrf_gpio_cfg_default(LED_2);
    nrf_gpio_cfg_default(LED_3);
    nrf_gpio_cfg_default(2);
    nrf_gpio_cfg_default(KEY_NETWORK);
    nrf_gpio_cfg_default(3);
    nrf_gpio_cfg_default(5);

    return ;
}
/**
* @function:tuya_remind_led_cntl
* @brief: control green LED flash
* @param[in]: none
* @return: none
*/
static void tuya_remind_led_cntl(void)
{
    nrf_gpio_cfg_default(LED_3);
    nrf_gpio_cfg_output(LED_1);
    nrf_gpio_cfg_output(LED_2);
    /* open LED */
    nrf_gpio_pin_write(LED_1, 1);
    nrf_gpio_pin_write(LED_2, 0);
    tuya_ble_device_delay_ms(100);
    /* close LED */
    nrf_gpio_pin_write(LED_1, 1);
    nrf_gpio_pin_write(LED_2, 1);
    tuya_ble_device_delay_ms(200);

    return ;
}
void tuya_remind_ble_connect_cb(void)
{
    int ble_status = 0;
    nrf_saadc_value_t key_val = 0;

    ble_status = tuya_ble_connect_status_get(); // get ble connect status
    if (BONDING_CONN == ble_status) {   // connect
        nrf_gpio_pin_clear(LED_3);
        nrf_gpio_pin_write(LED_1, 1);
        nrf_gpio_pin_write(LED_2, 1); // always close 
    } else {    // disconnect
        tuya_remind_led_cntl();  // flash   
    }
    /* long press 3s distribution network */
    nrf_drv_saadc_sample_convert(ADC_CHANNEL2, &key_val);   // adc value about 533 when key is no pressed 
    if (key_val > KEY_VAL_BOX1) {
        tuya_ble_device_delay_ms(3000);
    }
    nrf_drv_saadc_sample_convert(ADC_CHANNEL2, &key_val);
    if (key_val > KEY_VAL_BOX1) {
        tuya_ble_device_factory_reset();    // clear information about bound device
    }

}
/**
* @function:tuya_remind_ble_connect
* @brief: ble connect status LED(green)
* @param[in]: none
* @return: none
*/
int tuya_remind_ble_connect(void)
{
	tuya_ble_timer_create(&ble_conn_timer, 500, TUYA_BLE_TIMER_REPEATED, tuya_remind_ble_connect_cb);
	tuya_ble_timer_start(ble_conn_timer);

    return 0;
}
void tuya_remind_ble_conn_stop(void)
{
    tuya_ble_timer_stop(ble_conn_timer);
}
/**
* @function:tuya_remind_box_find
* @brief: called in main loop when flag_handle.find aignal changes to 1
* @param[in]: none
* @return: none
*/
int tuya_remind_box_find(void)
{
    int i = 0;
    static uint32_t sn = 0;

    // while (flag_handle.find) {   // wait for the DP_BOX_FIND command to send from the APP
    for (i = 0; i < 50; i++) { 
        tuya_beep_box_find_play(BEEP_HZ, 50);
        tuya_ble_device_delay_ms(200);
        tuya_beep_stop_play();
        tuya_ble_device_delay_ms(1000);
        
        while (BOX_OPEN == nrf_gpio_pin_read(KEY_OPEN) ) {
            tuya_beep_stop_play();
            find_buf[4] = 0;
            tuya_ble_dp_data_send(sn++, DP_SEND_TYPE_ACTIVE, DP_SEND_FOR_CLOUD_PANEL, DP_SEND_WITHOUT_RESPONSE, find_buf, DP_BUF_LEN(find_buf));

            break;
        }
    }
    flag_handle.find = 0;
    // }
    return 0;
}
/**
* @function:tuya_remind_start_find
* @brief: when send DP_BOX_FIND commond flag_handle.find aignal changes to 1
* @param[in]: none
* @return: none
*/
void tuya_remind_start_find(void)
{
    flag_handle.find = 1;

    return ;
}
static void enter_sleep_cb()
{
    ty_uart_uninit();
    ty_oled_clear();
    ty_i2c_uninit();

    tuya_beep_uninit();
    tuya_batmon_batval_uninit();
    tuya_remind_box_exit();

}
/**
* @function:tuya_remind_enter_sleep
* @brief: enter low power mode 
* @param[in]: none
* @return: none
*/
void tuya_remind_enter_sleep(void)
{
    tuya_ble_timer_t sleep_timer;

	tuya_ble_timer_create(&sleep_timer, 2000, TUYA_BLE_TIMER_SINGLE_SHOT, enter_sleep_cb);
	tuya_ble_timer_start(sleep_timer);

    return ;
}

/**
* @function:tuya_remind_wake_up
* @brief: wake up from low power mode
* @param[in]: none
* @return: none
*/
void tuya_remind_wake_up(void)
{
    ty_uart_init();
    tuya_remind_box_init(); 
    ty_ble_start_adv();

    return ;
}
void tuya_remind_key_sacn_cb(void)
{
    nrf_saadc_value_t key_val = 0;

    nrf_drv_saadc_sample_convert(ADC_CHANNEL2, &key_val);   // get adc value
    TUYA_APP_LOG_INFO("key_val = %d", key_val);
    if (key_val > KEY_VAL_BOX1) {
        // 药仓1药品已服用
    } else if (key_val < KEY_VAL_BOX1 && key_val > KEY_VAL_BOX2) {
        // 药仓2药品已服用
    } else if (key_val < KEY_VAL_BOX2 && key_val > KEY_VAL_BOX3) {
        // 药仓3药品已服用
    }

}
/**
* @function:tuya_remind_key_scan
* @brief: called in main loop when flag_handle.key_scan aignal changes to 1
* @param[in]: none
* @return: none
*/
void tuya_remind_key_scan(void)
{
    tuya_ble_timer_t key_scan_timer;

    tuya_ble_timer_create(&key_scan_timer, 200, TUYA_BLE_TIMER_REPEATED, tuya_remind_key_sacn_cb);
    tuya_ble_timer_start(key_scan_timer);

    return ;
}
/**
* @function:tuya_remind_small_mode_led
* @brief: LED for small box mode
* @param[in]: none
* @return: none
*/
static void tuya_remind_small_mode_led(void)
{
    int i = 0;

    for (i = 0; i < 20; i++) {
        /* LED_1 */
        nrf_gpio_cfg_default(LED_3);
        nrf_gpio_cfg_output(LED_1);
        nrf_gpio_cfg_output(LED_2);
        /* open LED */
        nrf_gpio_pin_write(LED_1, 1);
        nrf_gpio_pin_write(LED_2, 0);
        tuya_ble_device_delay_ms(3);
        /* close LED */
        nrf_gpio_pin_write(LED_1, 1);
        nrf_gpio_pin_write(LED_2, 1);
        /* LED_2 */
        nrf_gpio_cfg_default(LED_1);
        nrf_gpio_cfg_output(LED_2);
        nrf_gpio_cfg_output(LED_3);

        nrf_gpio_pin_write(LED_2, 1);
        nrf_gpio_pin_write(LED_3, 0);
        tuya_ble_device_delay_ms(3);

        nrf_gpio_pin_write(LED_2, 1);
        nrf_gpio_pin_write(LED_3, 1);
        /* LED_3 */
        nrf_gpio_cfg_default(LED_2);
        nrf_gpio_cfg_output(LED_1);
        nrf_gpio_cfg_output(LED_3);

        nrf_gpio_pin_write(LED_1, 1);
        nrf_gpio_pin_write(LED_3, 0);
        tuya_ble_device_delay_ms(3);

        nrf_gpio_pin_write(LED_1, 1);
        nrf_gpio_pin_write(LED_3, 1);
    }

    return ;
}
/**
* @function:tuya_remind_large_mode_led
* @brief: LED for lagre box mode 
* @param[in]: none
* @return: none
*/
static void tuya_remind_large_mode_led(void)
{
    int i = 0;

    for (i = 0; i < 20; i++) {
        /* LED_1 */
        nrf_gpio_cfg_default(LED_1);
        nrf_gpio_cfg_output(LED_2);
        nrf_gpio_cfg_output(LED_3);
        /* open LED */
        nrf_gpio_pin_write(LED_2, 0);
        nrf_gpio_pin_write(LED_3, 2);
        tuya_ble_device_delay_ms(5);
        /* close LED */
        nrf_gpio_pin_write(LED_2, 1);
        nrf_gpio_pin_write(LED_3, 1);
        /* LED_2 */
        nrf_gpio_cfg_default(LED_3);
        nrf_gpio_cfg_output(LED_1);
        nrf_gpio_cfg_output(LED_2);

        nrf_gpio_pin_write(LED_2, 1);
        nrf_gpio_pin_write(LED_1, 0);
        tuya_ble_device_delay_ms(5);

        nrf_gpio_pin_write(LED_2, 1);
        nrf_gpio_pin_write(LED_3, 1);
    }

    return ;
}
/**
* @function:tuya_remind_box_mode_led_play
* @brief: LED modes for different boxes
* @param[in]:  mode -> LARGE 2 boxes
                    -> SMALL 3 boxes
* @return: none
*/
int tuya_remind_box_mode_led_play(uint8_t mode)
{
    if (mode > 1) {
        TUYA_APP_LOG_ERROR("box mode select error");

        return TUYA_BLE_ERR_INVALID_PARAM;
    }

    int i, j;

    switch (mode) {
        case SMALL_BOX:
            for (i = 0; i < 3000; i++) {
                tuya_remind_small_mode_led();
            } 
        break;
        case LARGE_BOX:
            for (j = 0; j < 3000; j++) {
                tuya_remind_large_mode_led();
            }
        break;
        default:
        break;
    }
    
    return 0;
}
//static void tuya_remind_miss_alarm_cb()
//{
//    tuya_beep_medicine_alarm(BEEP_HZ, 50);
//    ty_oled_clear();
//	

//    return ;
//}
///**
//* @function:tuya_remind_miss_alarm
//* @brief: Miss remind if the first time not open box and eat medicine
//* @param[in]: none
//* @return: none
//*/
//void tuya_remind_miss_alarm(void)
//{    
//    tuya_ble_timer_create(&miss_alarm_timer, COUNT_TIME(3), TUYA_BLE_TIMER_SINGLE_SHOT, tuya_remind_miss_alarm_cb);
//    tuya_ble_timer_start(miss_alarm_timer);

//    return ;
//}
/**
* @function:tuya_remind_dose_show
* @brief: show the medicine num on OLED
* @param[in]: num_box1 -> the medicine num of box1
                num_box2 -> the medicine num of box2
                num_box3 -> the medicine num of box3
* @return: none
*/
void tuya_remind_dose_show(uint8_t num_box1, uint8_t num_box2, uint8_t num_box3)
{
    uint8_t box_mode = 0;

    ty_oled_clear();
    ty_oled_show_chinese(20, 0, 8); // 智
    ty_oled_show_chinese(40, 0, 9); // 能
    ty_oled_show_chinese(60, 0, 0); // 药
    ty_oled_show_chinese(80, 0, 1); // 盒

	ty_oled_show_chinese(16, 4, 10); 	// 仓
	ty_oled_show_num(33, 4, 1, 1, 16);	//  1
    ty_oled_show_chinese(48, 4, 10); 	// 仓
	ty_oled_show_num(65, 4, 2, 1, 16);	//  2
    ty_oled_show_chinese(80, 4, 10); 	// 仓
	ty_oled_show_num(98, 4, 3, 1, 16);	//  3
	/* dose of each box */
    ty_oled_show_num(20, 6, num_box1, 2, 16);
    ty_oled_show_num(52, 6, num_box2, 2, 16);
    ty_oled_show_num(84, 6, num_box3, 2, 16);
	/* get box mode */
	ty_flash_read(0x66000, &box_mode, 8);
    tuya_remind_box_mode_led_play(box_mode);
    ty_flash_erase(0x66000, 1);
	
    return ;
}



