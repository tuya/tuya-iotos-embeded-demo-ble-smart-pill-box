/**
* @file tuya_local_time.h
* @author zzw
* @email: huanling.zhang@tuya.com
* @brief tuya_local_time module is used to local timing on the device
* @version 0.1
* @date 2021-10-14
*
* @copyright Copyright (c) tuya.inc 2021
*
*/

#ifndef __TUYA_LOCAL_TIME_H__
#define __TUYA_LOCAL_TIME_H__
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
#define T_ENABLE    1
#define T_DISABLE   0
/*********************************************************************
****************************typedef define****************************
*********************************************************************/
typedef enum {
    Sun = 0,
    Mon,
    Tues,
    Wed,
    Thur,
    Fri,
    Sat,

} TY_WEEK_INFO_E;

/* valid agreement content */
typedef struct {
    uint8_t sw;    // Supports five alarm clocks   enable -> 128   disable -> 0
    uint8_t hour;
    uint8_t min;
    uint8_t week;      
    uint8_t box1;
    uint8_t box2;
    uint8_t box3;
} TY_CLOCK_INFO_T;

typedef struct {
    uint8_t num;
    TY_CLOCK_INFO_T clock[5];
} TY_CLOCK_LIST_T;
/*********************************************************************
****************************variable define***************************
*********************************************************************/


/*********************************************************************
****************************function define***************************
*********************************************************************/
/**
* @function:tuya_local_time_parse
* @brief: parse DP_MEDICINE_CLOCK send dp data
* @param[in]: dp_data -> DP_MEDICINE_CLOCK send dp data
* @param[in]: len -> the DP_MEDICINE_CLOCK len from dp_value
* @return: success -> 0   fail -> else
*/
int tuya_local_time_parse(uint8_t *dp_data, uint8_t len);
int tuya_local_time_process(void);
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*__TUYA_LOCAL_TIME_H__*/


