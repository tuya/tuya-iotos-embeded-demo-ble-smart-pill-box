/**
* @file tuya_dp_process.h
* @author zzw
* @email: huanling.zhang@tuya.com
* @brief tuya_dp_process module is used to update dp data
* @version 0.1
* @date 2021-10-15
*
* @copyright Copyright (c) tuya.inc 2021
*
*/

#ifndef __TUYA_DP_PROCESS_H__
#define __TUYA_DP_PROCESS_H__
/*********************************************************************
********************************includes******************************
*********************************************************************/
#include <stdint.h>

/*********************************************************************
****************************private includes**************************
*********************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*********************************************************************
******************************macro define****************************
*********************************************************************/
#define     ALARM   1
#define     NORMAL  0

/* DP_ID */
#define     DP_BOX_FIND         102    // Bool
#define     DP_BOX_MODE         17     // Enum
#define     DP_MISS_REMIND      26     // Bool
#define     DP_MEDICINE_CLOCK   14     // Raw
#define     DP_LOW_POWER_ALARM  24     // Bool
#define     DP_MEDICINE_REMIND  27     // Bool

/*********************************************************************
****************************typedef define****************************
*********************************************************************/
typedef enum {
    RAW = 0,
    BOOL,
    VALUE,
    STRING,
    ENUM
} TY_DP_TYPE_E;
/*********************************************************************
****************************variable define***************************
*********************************************************************/


/*********************************************************************
****************************function define***************************
*********************************************************************/
/**
* @function:tuya_dp_revice_handle
* @brief: processing data from APP 
* @param[in]: dp_data -> recive dp buf
* @param[in]: len -> the dp total length
* @return: none
*/
int tuya_dp_revice_handle(uint8_t *dp_data, uint16_t len);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*__TUYA_DP_PROCESS_H__*/
