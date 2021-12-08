/**
* @file tuya_remind.h
* @author zzw
* @email: huanling.zhang@tuya.com
* @brief tuya_remind  module is used to reminders related functions
* @version 0.1
* @date 2021-10-14
*
* @copyright Copyright (c) tuya.inc 2021
*
*/

#ifndef __TUYA_REMIND_H__
#define __TUYA_REMIND_H__
/*********************************************************************
********************************includes******************************
*********************************************************************/
#include "stdint.h"
#include "ty_i2c.h"
#include "ty_rtc.h"
#include "ty_ble.h"
#include "ty_oled.h"
#include "ty_uart.h"
#include "ty_flash.h"
#include "nrf_gpio.h"
#include "nrf_saadc.h"
#include "tuya_ble_log.h"
#include "nrf_drv_saadc.h"
#include "tuya_ble_type.h"
#include "tuya_ble_type.h"
/*********************************************************************
****************************private includes**************************
*********************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*********************************************************************
******************************macro define****************************
*********************************************************************/
/* box status */
#define     BOX_OPEN    0
#define     BOX_CLOSE   1

/* box mode */
#define     LARGE_BOX     1
#define     SMALL_BOX     0

/* pin port */
#define     LED_1           15
#define     LED_2           14
#define     LED_3           11
#define     KEY_OLED        20
#define     KEY_OPEN        19
#define     LED_SWITCH      16
#define     KEY_NETWORK      4
#define     ADC_CHANNEL0     0      // channel 0 -> GPIO_2
#define     ADC_CHANNEL2     2      // channel 2 -> GPIO_4 由于4口复用为ADC口和普通IO口因此在每次需要ADC采集时先初始化、采集完成后反初始化，在配网时初始化为普通IO口

/* adc value of the box key */
#define     KEY_VAL_BOX1    930
#define     KEY_VAL_BOX2    460
#define     KEY_VAL_BOX3    225

#define     COUNT_TIME(x)   (1000 * 60 * x)     // unit:min
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
* @function:tuya_remind_box_init
* @brief: medicine box init
* @param[in]: none
* @return: none
*/
void tuya_remind_box_init(void);
/**
* @function:tuya_remind_box_exit
* @brief: uninit box
* @param[in]: none
* @return: none
*/
void tuya_remind_box_exit(void);
/**
* @function:tuya_remind_ble_connect
* @brief: 
* @param[in]: none
* @return: none
*/
int tuya_remind_ble_connect(void);
/**
* @function:tuya_remind_box_find
* @brief: called in a loop when find_handle aignal changes to 1
* @param[in]: none
* @return: none
*/
int tuya_remind_box_find(void);
/**
* @function:tuya_remind_start_find
* @brief: when send DP_BOX_FIND commond find_handle aignal changes to 1
* @param[in]: none
* @return: none
*/
void tuya_remind_start_find(void);
/**
* @function:tuya_remind_enter_sleep
* @brief: enter low power mode 
* @param[in]: none
* @return: none
*/
void tuya_remind_enter_sleep(void);
/**
* @function:tuya_remind_wake_up
* @brief: wake up from low power mode
* @param[in]: none
* @return: none
*/
void tuya_remind_wake_up(void);
int tuya_remind_box_mode_led_play(uint8_t mode);
/**
* @function:tuya_remind_key_scan
* @brief: called in main loop when flag_handle.key_scan aignal changes to 1
* @param[in]: none
* @return: none
*/
void tuya_remind_key_scan(void);
void tuya_remind_miss_alarm(void);
void tuya_remind_dose_show(uint8_t num_box1, uint8_t num_box2, uint8_t num_box3);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*__TUYA_REMIND _H__*/
