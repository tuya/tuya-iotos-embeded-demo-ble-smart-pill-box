# Tuya IoTOS Embeded Demo BLE Smart Pill Box

[English](./README.md) | [中文](./README_zh.md)

<br>

## 简介 

本 demo 基于涂鸦智能云平台、涂鸦智能APP、IoTOS Embeded Ble SDK，使用涂鸦BLE系列模组开发出一款智能药盒demo。功能包括：定时提醒，药盒查找、服药提醒、错过提醒、低电量报警。

<br>

## 快速上手

### 开发环境搭建 

- IDE 根据芯片原厂 SDK 要求进行安装。

- Tuya BLE SDK Demo Project 下载地址见下表所示。请参考各分支下的 `README.md` 文件完成工程导入。

  |   芯片平台   |   型号   |                           下载地址                           |
  | :----------: | :------: | :----------------------------------------------------------: |
  |    Nordic    | nrf52832 | [tuya_ble_sdk_Demo_Project_nrf52832.git](https://github.com/TuyaInc/tuya_ble_sdk_Demo_Project_nrf52832.git) |
  |   Realtek    | RTL8762C | [tuya_ble_sdk_Demo_Project_rtl8762c.git](https://github.com/TuyaInc/tuya_ble_sdk_Demo_Project_rtl8762c.git) |
  |    Telink    | TLSR825x | [tuya_ble_sdk_Demo_Project_tlsr8253.git](https://github.com/TuyaInc/tuya_ble_sdk_Demo_Project_tlsr8253.git) |
  | Silicon Labs |   BG21   |                     正在规划中，敬请期待                     |
  |    Beken     | BK3431Q  | [Tuya_ble_sdk_demo_project_bk3431q.git](https://github.com/TuyaInc/Tuya_ble_sdk_demo_project_bk3431q.git) |
  |    Beken     |  BK3432  | [ tuya_ble_sdk_Demo_Project_bk3432.git](https://github.com/TuyaInc/tuya_ble_sdk_Demo_Project_bk3432.git) |
  |   Cypress    |  Psoc63  | [tuya_ble_sdk_Demo_Project_PSoC63.git](https://github.com/TuyaInc/tuya_ble_sdk_Demo_Project_PSoC63.git) |

<br>

### 编译烧录

- 代码修改

  1. 在 `tuya_ble_sdk_demo.h` 填入在涂鸦 IoT 工作台创建的智能产品 PID。

     ```
      #define TY_DEVICE_PID     "xxxxxxxx"
     ```

     将其中 `xxxxxxxx` 替换为PID。

  2. 在 `tuya_ble_app_demo.c` 填入申请的授权码（uuid 和 auth key）。

     ```
     #define TY_DEVICE_DID         "zzzzzzzz" //16Bytes
     #define TY_DEVICE_AUTH_KEY    "yyyyyyyy" //32Bytes
     ```

     其中 `yyyyyyyy` 替换为 auth key， `zzzzzzzz` 替换为 uuid.

- 编译运行Demo代码

  修改好后编译代码，并下载固件至硬件运行（根据所选的芯片型号有可能还需要下载 stack 和 bootloader），观察 log 日志 ，并用第三方蓝牙调试 App（例如 IOS 下的 lightBlue）扫描确认设备有没有正常广播。

<br>

### 文件介绍

```shell
Application/
├── include
│   ├── tuya_battery_monitor.h
│   ├── tuya_beep.h
│   ├── tuya_dp_process.h
│   ├── tuya_local_time.h
│   └── tuya_remind.h
└── src
    ├── tuya_battery_monitor.c
    ├── tuya_beep.c
    ├── tuya_dp_process.c
    ├── tuya_local_time.c
    └── tuya_remind.c
```

<br>

### 应用入口

入口文件：/tuya_ble_app/tuya_ble_app_main.c

+ `void tuya_ble_sdk_demo_init(void)` 对Tuya IoTOS Embeded Ble SDK 进行一些必要的初始化，该函数只执行一次。

<br>

### DP点相关 

|  函数名  | **tuya_ble_dp_data_send**                                    |
| :------: | :----------------------------------------------------------- |
| 函数原型 | tuya_ble_status_t tuya_ble_dp_data_send(<br/>uint32_t sn,<br/>tuya_ble_dp_data_send_type_t type,<br/>tuya_ble_dp_data_send_mode_t mode,<br/>tuya_ble_dp_data_send_ack_t ack,<br/>uint8_t *p_dp_data,<br/>uint32_t dp_data_len<br/>) ; |
| 功能概述 | 发送 DP 数据                                                 |
|   参数   | sn[in]：发送序号<br/>type[in]：发送类型，分为主动发送和应答查询发送<br/>mode[in]：发送模式<br/>ack[in]：是否需要应答标志<br/>p_dp_data [in]：DP 数据<br/>dp_data_len[in]：数据长度，最大不能超过 `TUYA_BLE_SEND_MAX_DATA_LEN-7`。其中 `TUYA_BLE_SEND_MAX_DATA_LEN `可配置 |
|  返回值  | `TUYA_BLE_SUCCESS`：发送成功<br/>`TUYA_BLE_ERR_INVALID_PARAM`：参数无效<br/>`TUYA_BLE_ERR_INVALID_STATE`：当前状态不支持发送，例如蓝牙断开<br/>`TUYA_BLE_ERR_NO_MEM`：内存申请失败<br/>`TUYA_BLE_ERR_INVALID_LENGTH`：数据长度错误<br/>`TUYA_BLE_ERR_NO_EVENT`：其他错误 |
|   备注   | 应用程序通过调用该函数上报 DP 数据到 App。                   |

参数说明：

涂鸦云平台是以 dp 点的方式管理数据，任何设备产生的数据都需要抽象为 dp 点的形式，一个完整的dp点数据由四部分组成（具体参考 IoT 工作台上的相关介绍）：

- Dp_id： 1个字节，在开发平台注册的 dp_id 序号。


- Dp_type：1 个字节，dp点类型。

  ```c
  #define DT_RAW    0   //raw类型；
  
  #define DT_BOOL   1   // 布尔类型；
  
  #define DT_VALUE  2   //数值类型；（其范围在iot平台注册时指定）
  
  #define DT_STRING 3   //字符串类型；
  
  #define DT_ENUM   4   // 枚举类型；
  
  #define DT_BITMAP 5   //位映射类型；
  ```

- Dp_len：1 个字节或者两个字节。


- Dp_data：数据，dp_len 个字节。


该 dp 点上报函数的参数 p_data 指向的数据必须以下表格式组装上报：

| Dp点1的数据 |         |        |        |         | ~    | Dp点n的数据 |         |        |         |
| :---------: | :-----: | :----: | ------ | :-----: | :--- | :---------: | :-----: | :----: | :-----: |
|      1      |    2    |   3    | 4      |   5~    | ~    |      n      |   n+1   |  n+2   |  n+3~   |
|    Dp_id    | Dp_type | Dp_len | Dp_len | Dp_data | ~    |    Dp_id    | Dp_type | Dp_len | Dp_data |

调用该函数时，参数 len 的最大长度为 `TUYA_BLE_REPORT_MAX_DP_DATA_LEN`（当前为255+3）。



## 相关文档

- [BLE SDK 说明](https://developer.tuya.com/cn/docs/iot/device-development/embedded-software-development/module-sdk-development-access/ble-chip-sdk/tuya-ble-sdk-user-guide?id=K9h5zc4e5djd9#title-17-tuya%20ble%20sdk%20callback%20event%20%E4%BB%8B%E7%BB%8D) 
- [BLE SDK Demo 说明](https://developer.tuya.com/cn/docs/iot/device-development/embedded-software-development/module-sdk-development-access/ble-chip-sdk/tuya-ble-sdk-demo-instruction-manual?id=K9gq09szmvy2o) 
- [涂鸦 Demo 中心](https://developer.tuya.com/cn/demo) 

<br>


## 技术支持

您可以通过以下方法获得涂鸦的支持:

- 开发者中心：https://developer.tuya.com/cn/
- 帮助中心：https://support.tuya.com/help
- 技术支持工单中心：https://service.console.tuya.com

<br>