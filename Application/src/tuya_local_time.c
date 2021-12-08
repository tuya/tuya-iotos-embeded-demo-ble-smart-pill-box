/**
* @file tuya_local_time.c
* @author zzw
* @email: huanling.zhang@tuya.com
* @brief tuya_local_time module is used to local timing on the device
* @version 0.1
* @date 2021-10-14
*
* @copyright Copyright (c) tuya.inc 2021
*
*/

/*********************************************************************
********************************includes******************************
*********************************************************************/
#include "ty_rtc.h"
#include "tuya_ble_log.h"
/*********************************************************************
****************************private includes**************************
*********************************************************************/
#include "tuya_local_time.h" 
/*********************************************************************
******************************macro define****************************
*********************************************************************/
#define     WEEK_COUNT      1   
#define     ASCII_DEC_0     48  
#define     PER_CLOCK_LEN   27      // The length of each alarm data frame
#define     PER_BOX_MEDICINE(bit_H, bit_L)      (((bit_H - ASCII_DEC_0) * 10) + (bit_L - ASCII_DEC_0))
//#define     COUNT_TIME(x)   (1000 * 60 * x)     // unit:min
//#define     DP_BUF_LEN(buf) (sizeof(buf) / sizeof(char))  
/*********************************************************************
****************************typedef define****************************
*********************************************************************/

/* agreement content */
#pragma(1)

typedef struct {
    uint8_t sw; // on_off
    uint8_t week;
    uint8_t hour;
    uint8_t min;
    uint8_t action;
    uint8_t resv_1;
    uint8_t resv_2;
    uint8_t box_dose[18];
    uint8_t resv_3;
    uint8_t resv_4;
}TY_SIGEL_TIME_PRO_T;

typedef struct{
    uint8_t ver;
    TY_SIGEL_TIME_PRO_T singel_time[5];
}TY_TIME_PRO_T;

#pragma()
/*********************************************************************
****************************variable define***************************
*********************************************************************/
#if WEEK_COUNT
static TY_CLOCK_LIST_T medic_info;
// static TY_MEDICINE_CLOCK_INFO_T medicine_info;
#endif
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
int tuya_local_time_parse(uint8_t *dp_data, uint8_t len)
{
    if (NULL == dp_data) {
        TUYA_APP_LOG_ERROR("invalid parse dp_data!!!");
        return TUYA_BLE_ERR_INVALID_PARAM;
    }

    #if 0   //if want to see dp data from APP send open this macro when debug
    for (int a = 0; a < len; a++) {
        TUYA_APP_LOG_INFO("time_data[%d] = %d", a, dp_data[a]);
    }
    #endif

    int i = 0;
    int time_cnt = 0;
    TY_TIME_PRO_T *time_pro = NULL;

    time_pro = (TY_TIME_PRO_T *)dp_data;
    time_cnt = (len - 1) / sizeof(TY_SIGEL_TIME_PRO_T);   // (len - 1) -> (clock data) - (1 byte clock version)
    if (time_cnt > 5) {     // The number of alarm clocks exceeds the upper limit
        TUYA_APP_LOG_ERROR("More than 5 alarm clocks");
        return TUYA_BLE_ERR_INVALID_PARAM;
    } else if (0 == time_cnt) { // alarm clock num is 0
        TUYA_APP_LOG_INFO("No alarm clock is available");
        return 0;
    }
    /* parse version number */
    if (time_pro->ver != 0) {
        TUYA_APP_LOG_ERROR("Invalid timing version!");
        return TUYA_BLE_ERR_INTERNAL;
    }
    /* parse 'on_off'、week、hour、minute、dose */
    medic_info.num = time_cnt;
    for (i = 0; i < medic_info.num; i++) {
        medic_info.clock[i].sw = time_pro->singel_time[i].sw;
        medic_info.clock[i].week = time_pro->singel_time[i].week;
        medic_info.clock[i].hour = time_pro->singel_time[i].hour;
        medic_info.clock[i].min = time_pro->singel_time[i].min;
        medic_info.clock[i].box1 = PER_BOX_MEDICINE(time_pro->singel_time[i].box_dose[1], (time_pro->singel_time[i].box_dose[2]));
        medic_info.clock[i].box2 = PER_BOX_MEDICINE(time_pro->singel_time[i].box_dose[4], (time_pro->singel_time[i].box_dose[5]));
        medic_info.clock[i].box3 = PER_BOX_MEDICINE(time_pro->singel_time[i].box_dose[7], (time_pro->singel_time[i].box_dose[8]));
    }
        
    return TUYA_BLE_SUCCESS;
}
/**
* @function:tuya_local_time_process
* @brief: in the loop
* @param[in]: none
* @return: none
*/
int tuya_local_time_process(void)
{
    ty_rtc_wakeup_event_process(&medic_info);

    return 0;
}

#if 0
    /* parse version number */
    version_num = dp_data[0];
    if (version_num != 0) {
        TUYA_APP_LOG_ERROR("Invalid timing version!");
        return TUYA_BLE_ERR_INTERNAL;
    }
    /* parse 'clock_enable'、hour、minute、midicine_num data */
    medicine_info.alarm_clock_num = (len - 1) / PER_CLOCK_LEN;    // (len - 1) -> (clock data) - (1 byte clock version)
    if (medicine_info.alarm_clock_num > 5) {
        TUYA_APP_LOG_ERROR("More than 5 alarm clocks");
        return TUYA_BLE_ERR_INVALID_PARAM;
    } else {
        TUYA_APP_LOG_INFO("There are %d alarm clocks!", medicine_info.alarm_clock_num);
    } 
    
    for (i = 0; i < medicine_info.alarm_clock_num; i++) { 
        /* close or open clock */  
        medicine_info.clock_enable[i] = dp_data[(PER_CLOCK_LEN * i) + 1];   // Location of each clock info (dp_data[1]、dp_data[28]、dp_data[55]、dp_data[82]、dp_data[109])
        /* hour */
        medicine_info.hour[i] = dp_data[(PER_CLOCK_LEN * i) + 3];
        /* minute */
        medicine_info.minute[i] = dp_data[(PER_CLOCK_LEN * i) + 4];
        /* b0x1~box3 medicine num */
        medicine_info.box1_medicine[i] = (dp_data[(PER_CLOCK_LEN * i) + 9] - ASCII_DEC_0)*10 + \
                                                                                (dp_data[(PER_CLOCK_LEN * i) + 10] - ASCII_DEC_0);   // dp_data存放的是字符对应的ASCII值(十进制)，需要用的是字符，因此减去48
        medicine_info.box2_medicine[i] = (dp_data[(PER_CLOCK_LEN * i) + 12] - ASCII_DEC_0)*10 + \
                                                                                (dp_data[(PER_CLOCK_LEN * i) + 13] - ASCII_DEC_0);
        medicine_info.box3_medicine[i] = (dp_data[(PER_CLOCK_LEN * i) + 15] - ASCII_DEC_0)*10 + \
                                                                                (dp_data[(PER_CLOCK_LEN * i) + 16] - ASCII_DEC_0);

    }
    /* parse week once or repeat */
    for (m = 0; m < medicine_info.alarm_clock_num; m++) { // Location of week info (dp_data[2]、 dp_data[29]、 dp_data[56]、 dp_data[83] 、dp_data[110])
        medicine_info.is_once_clock[m] = dp_data[(PER_CLOCK_LEN * m) + 2];
        for (j = 0; j < 8; j++) {
            if (dp_data[(PER_CLOCK_LEN * m) + 2] & (1 << j)) {
                medicine_info.week_buf[n] = 1;
            } else {
                medicine_info.week_buf[n] = 0;
            }
            n++;
        } 
    }
#endif








