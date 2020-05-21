/*
 * MCU C-SDK Ver:1.0.0
 * ©2020  MICHIP Technology inc.
 * https://github.com/xluohome/mcu_c-sdk
 */

#ifndef __MX_MCU_APP_H_
#define __MX_MCU_APP_H_

#define _XDATA //变量数据保存在xdata区，根据单片机资源选择
#ifdef _XDATA
#define SYSTEM_XDATA xdata
#else
#define SYSTEM_XDATA
#endif

#define WIFI_MCU_CONTROL_MODE  //重置wifi需要用到的api
#define WIFI_STATUS_ENABLE //wifi状态指示灯处理方式

/******************************************************************************
                         定义收发缓存                   
******************************************************************************/
#define WIFI_RX_BUF_SIZE 55 //串口数据接收缓存区大小,如MCU的RAM不够,可缩小
#define WIFI_TX_BUF_SIZE 55 //串口数据接收缓存区大小,如MCU的RAM不够,可缩小

/*****************************************************************************
函数名称 : wifi_uart_tx_data
功能描述 : 向wifi uart写入连续数据
输入参数 : ptr:发送缓存指针
           len:数据发送长度
返回参数 : 无
*****************************************************************************/
void wifi_uart_tx_data(unsigned char *ptr, unsigned short len);

/*****************************************************************************
函数名称 : mx_func_cmd_handle
功能描述 : 产品功能指令处理
输入参数 : value:下发数据源指针
返回参数 : ret:返回数据处理结果
*****************************************************************************/
unsigned char mx_func_cmd_handle(const unsigned char value[]);

/*****************************************************************************
函数名称 : mx_mcu_data_update
功能描述 : 数据上报更新
输入参数 : 无
返回参数 : 无
使用说明 : MCU 主循环while（1）周期调用处理，间隔不大于5ms
*****************************************************************************/
void mx_mcu_data_update(void);

#ifdef WIFI_STATUS_ENABLE
/*****************************************************************************
函数名称 : wifi_status_process
功能描述 : wifi状态处理
输入参数 : 无
返回参数 : 无
使用说明 : MCU 主循环while（1）周期调用处理，间隔不大于5ms
*****************************************************************************/
void wifi_status_handle(void);
#endif
/******************************************************************************
                         定义数据变量，请按照实际数据修改            
******************************************************************************/
extern SYSTEM_XDATA unsigned char power_switch;
extern SYSTEM_XDATA unsigned char fan_speed;
extern SYSTEM_XDATA unsigned char light;
extern SYSTEM_XDATA unsigned char keep_warm;
extern SYSTEM_XDATA unsigned char cleaning;

#endif
