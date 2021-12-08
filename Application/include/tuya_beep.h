/**
* @file tuya_beep.h
* @author zzw
* @email: huanling.zhang@tuya.com
* @brief tuya_beep module is used to control the buzzer sound
* @version 0.1
* @date 2021-10-12
*
* @copyright Copyright (c) tuya.inc 2021
*
*/

#ifndef __TUYA_BEEP_H__
#define __TUYA_BEEP_H__
/*********************************************************************
********************************includes******************************
*********************************************************************/
#include "stdint.h"

/*********************************************************************
****************************private includes**************************
*********************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*********************************************************************
******************************macro define****************************
*********************************************************************/
#define BUZZER_PIN	12
#define BEEP_HZ     8000 
/*********************************************************************
****************************typedef define****************************
*********************************************************************/


/*********************************************************************
****************************variable define***************************
*********************************************************************/

/*********************************************************************
****************************function define***************************
*********************************************************************/
/**
* @function:tuya_beep_init
* @brief: buzzer port init
* @param[in]: pin_num: buzzer pin port
* @return: none
*/
void tuya_beep_init(uint8_t pin_num);
/**
* @function:tuya_beep_medicine_alarm
* @brief: start pwm
* @param[in]: scale_freq -> set  frequency, unit:Hz  
* @param[in]: duty -> set pwm duty cycle (0 ~ 100)
* @return: none
*/
int tuya_beep_medicine_alarm(uint32_t scale_freq, uint8_t duty);
/**
* @function:tuya_beep_medicine_alarm_start
* @brief: 
* @param[in]: none
* @return: none
*/
void tuya_beep_medicine_alarm_start(void);
/**
* @function:tuya_beep_box_find_play
* @brief: start pwm
* @param[in]: scale_freq -> set  frequency, unit:Hz  
* @param[in]: duty -> set pwm duty cycle (0 ~ 100)
* @return: -1 -> fail
            0 -> success
*/
int tuya_beep_box_find_play(uint32_t scale_freq, uint8_t duty);
/**
* @function:tuya_beep_stop_play
* @brief: stop beep play
* @param[in]: none
* @return: none
*/
void tuya_beep_stop_play(void);
/**
* @function:tuya_beep_uninit
* @brief: uninit pwm
* @param[in]: none
* @return: none
*/
void tuya_beep_uninit(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*__TUYA_BEEP_H__*/
