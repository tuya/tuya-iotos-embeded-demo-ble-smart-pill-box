/**
* @file tuya_dp_process.c
* @author zzw
* @email: huanling.zhang@tuya.com
* @brief tuya_dp_process module is used to update dp data
* @version 0.1
* @date 2021-10-15
*
* @copyright Copyright (c) tuya.inc 2021
*
*/

/*********************************************************************
********************************includes******************************
*********************************************************************/
#include "ty_flash.h"
#include "tuya_ble_type.h"
/*********************************************************************
****************************private includes**************************
*********************************************************************/
#include "tuya_beep.h"
#include "tuya_remind.h"
#include "tuya_dp_process.h"
#include "tuya_local_time.h"
/*********************************************************************
******************************macro define****************************
*********************************************************************/


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
* @function:tuya_dp_revice_handle
* @brief: processing data from APP 
* @param[in]: dp_data -> recive dp buf
* @param[in]: len -> the dp total length
* @return: none
*/
int tuya_dp_revice_handle(uint8_t *dp_data, uint16_t len)
{
    if (NULL == dp_data || len > 255) {
        TUYA_APP_LOG_ERROR("invalid app dp_data param!!!");

        return TUYA_BLE_ERR_INVALID_PARAM;
    }

    uint8_t dp_id = 0;
    
    uint8_t time_data[255];

    #if 0  // if want to see dp data from APP send open this macro when debug
        int i = 0;
        for (i = 0; i < len; i++) {
            TUYA_APP_LOG_INFO("dp_data[%d]%d", i, dp_data[i]);
        }
    #endif

    dp_id = dp_data[0];
	if (DP_MEDICINE_CLOCK == dp_id) {       // memcpy DP_value to time_data when DP_type is raw(DP_MEDICINE_CLOCK)
		memset(time_data, 0, sizeof(time_data));
		memcpy(time_data, &dp_data[4], len - 4);
	}

    switch (dp_id) { 
        case DP_MEDICINE_CLOCK:  
        tuya_local_time_parse(&dp_data[4], len - 4); 
        break;
        case DP_BOX_FIND:
            if (1 == dp_data[4]) {
                tuya_remind_start_find();
            } else if (0 == dp_data[4]) {
                tuya_beep_stop_play();
            }           
        break;
        case DP_MEDICINE_REMIND:
            if (0 == dp_data[4]) {
                tuya_beep_stop_play();
            } 
        case DP_BOX_MODE:   // box mode change
            ty_flash_write(0x66000, &dp_data[4], 8);    // write box_mode to flash
        break;
        default:
        break;
    }

	return 0;
	
}


