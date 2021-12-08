/**
* @file tuya_battery_monitor.h
* @author zzw
* @email: huanling.zhang@tuya.com
* @brief tuya_battery_monitor module is used to detect battery power
* @version 0.1
* @date 2021-08-26
*
* @copyright Copyright (c) tuya.inc 2021
*
*/

#ifndef __TUYA_BATTERY_MONITOR_H__
#define __TUYA_BATTERY_MONITOR_H__
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
#define ADC_SELECT  TY_GPIOA_22

/*********************************************************************
****************************typedef define****************************
*********************************************************************/
typedef enum {
   pet_10 = 3700,
   pet_20 = 3750,
   pet_30 = 3800,
   pet_40 = 3850,
   pet_50 = 3900,
   pet_60 = 3950,
   pet_70 = 4000,
   pet_80 = 4050,
   pet_90 = 4100,
   pet_100 = 4150
}TY_BAT_VALUE_E;

typedef enum {
    _10p = 0,    
    _20p,
    _30p,
    _40p,
    _50p,
    _60p,
    _70p,
    _80p,
    _90p,
    _100p,
}TY_OLED_DISPLAY_E;
/*********************************************************************
****************************variable define***************************
*********************************************************************/

/*********************************************************************
****************************function define***************************
*********************************************************************/
/**
* @function:tuya_batmon_batval_init
* @brief: adc config
* @param[in]: adc_channel -> choose adc channel
* @return: none
*/
int tuya_batmon_batval_init(uint8_t adc_channel);
/**
* @function:tuya_batmon_bat_level_report
* @brief: get battery value, report to APP
* @param[in]: none
* @return: none
*/
int tuya_batmon_bat_level_report(void);
/**
* @function:tuya_batmon_batval_uninit
* @brief: cancel adc
* @param[in]: none
* @return: none
*/
void tuya_batmon_batval_uninit(void);


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*__TUYA_BATTERY_MONITOR_H__*/








