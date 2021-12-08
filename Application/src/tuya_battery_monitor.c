/**
* @file tuya_battery_monitor.c
* @author zzw
* @email: huanling.zhang@tuya.com
* @brief tuya_battery_monitor module is used to detect battery power
* @version 0.1
* @date 2021-08-26
*
* @copyright Copyright (c) tuya.inc 2021
*
*/

/*********************************************************************
********************************includes******************************
*********************************************************************/
#include "tuya_ble_api.h"
/*********************************************************************
****************************private includes**************************
*********************************************************************/
#include "tuya_remind.h"
#include "tuya_dp_process.h"
#include "tuya_battery_monitor.h"
/*********************************************************************
******************************macro define****************************
*********************************************************************/
#define     DP_BUF_LEN(buf) (sizeof(buf) / sizeof(char)) 
/*********************************************************************
****************************typedef define****************************
*********************************************************************/


/*********************************************************************
****************************variable define***************************
*********************************************************************/
static uint8_t battery_buf[5] = {DP_LOW_POWER_ALARM, BOOL, 0x00, 0x01, 0x00};    // {DP_ID, DP_type, [2][3]->DP_len, DP_data}
static nrf_saadc_channel_config_t adc_bat = NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_AIN0);
/*********************************************************************
****************************function define***************************
*********************************************************************/
static void tuya_remind_key_cb(nrf_drv_saadc_evt_t const *p_event)
{
}
/**
* @function:tuya_batmon_batval_init
* @brief: adc config
* @param[in]: adc_channel -> choose adc channel
* @return: none
*/
int tuya_batmon_batval_init(uint8_t adc_channel)
{
    nrf_drv_saadc_init(NULL, tuya_remind_key_cb);   // NULL -> use default configuration
    nrf_drv_saadc_channel_init(adc_channel, &adc_bat);

    return 0;
}
/**
* @function:tuya_batmon_BatVal_get
* @brief: get battery value, called in a loop when adc_flag changes to 1
* @param[in]: vref -> adc reference voltage  unit:mv
* @param[in]: sample -> adc sampling accuracy 
* @param[in]: ratio -> partial voltage ratio of power supply circuit  
* @return: Bat_val
*/
static uint16_t tuya_batmon_batval_get(uint16_t vref, uint16_t sample, float ratio)
{
    int i = 0;
    uint16_t Bat_val = 0;
    uint16_t adc_Avg = 0;
    uint16_t voltage_val = 0;
    nrf_saadc_value_t val_bat = 0;

    for (i = 0; i < 5; i++) {
        nrf_drv_saadc_sample_convert(ADC_CHANNEL0, &val_bat);
        adc_Avg += val_bat;
    }
    adc_Avg /= 5;
    voltage_val = (adc_Avg * vref) / sample;   // ADC Sample 10bit Ref voltage 3.6V
    Bat_val = voltage_val * ratio;  // Voltage divider The actual battery voltage value is 2 times the voltage value taken by the adc  unit:mv
    // TUYA_APP_LOG_INFO("battery_val:%dmv", Bat_val);

    return Bat_val;
}
/**
* @function:tuya_batmon_bat_level_report
* @brief: get battery value, report to APP
* @param[in]: none
* @return: none
*/
int tuya_batmon_bat_level_report(void)
{
    uint8_t i = 0;
    uint8_t op_ret;
    uint16_t Bat_val = 0;
    static uint32_t sn = 0;

    for (i = 0; i < 5; i++) {
        Bat_val = tuya_batmon_batval_get(3600, 1024, 2);
        if (Bat_val <= pet_10) {
            battery_buf[4] = ALARM;
            nrf_gpio_pin_write(LED_SWITCH, 1);  // low power, red light
            op_ret = tuya_ble_dp_data_send(sn++, DP_SEND_TYPE_ACTIVE, DP_SEND_FOR_CLOUD_PANEL, \
											DP_SEND_WITHOUT_RESPONSE, battery_buf, DP_BUF_LEN(battery_buf));    
        } else {
            battery_buf[4] = NORMAL;
            nrf_gpio_pin_write(LED_SWITCH, 0);  // enough power, green light
            op_ret = tuya_ble_dp_data_send(sn++, DP_SEND_TYPE_ACTIVE, DP_SEND_FOR_CLOUD_PANEL,\
											DP_SEND_WITHOUT_RESPONSE, battery_buf, DP_BUF_LEN(battery_buf));
        }
        tuya_ble_device_delay_ms(200);
    }

    return 0;
}
/**
* @function:tuya_batmon_batval_uninit
* @brief: cancel adc
* @param[in]: none
* @return: none
*/
void tuya_batmon_batval_uninit(void)
{
    nrf_drv_saadc_uninit();

    return ;
}






    