/*
 * MCU C-SDK Ver:1.0.0
 * ©2020  MICHIP Technology inc.
 * https://github.com/xluohome/mcu_c-sdk
 */

/************************请勿修改以下代码*****************************/

#ifndef __MX_UART_API_H_
#define __MX_UART_API_H_

#include "mx_mcu_app.h"

//=============================================================================
/*定义常量*/
//=============================================================================
#ifndef TRUE
#define TRUE 1
#endif
//
#ifndef FALSE
#define FALSE 0
#endif
//
#ifndef NULL
#define NULL ((void *)0)
#endif

#ifndef SUCCESS
#define SUCCESS 1
#endif

#ifndef ERROR
#define ERROR 0
#endif

//=============================================================================
//数据类型
//=============================================================================
#define DATA_TYPE_BOOL 0x00   //布尔型
#define DATA_TYPE_ENUM 0x02   //枚举型
#define DATA_TYPE_INT 0x05    //int型
#define DATA_TYPE_FLOAT 0x09  //float型
#define DATA_TYPE_STRING 0x0b //字符串型

//=============================================================================
//WIFI工作状态
//=============================================================================
#define SMART_CONFIG_STATE 0x00
#define AP_STATE 0x01
#define WIFI_NOT_CONNECTED 0x02
#define WIFI_CONNECTED 0x03
#define WIFI_CONN_CLOUD 0x04
#define WIFI_LOW_POWER 0x05
//=============================================================================
//wifi配网的方式
//=============================================================================
#define SMART_CONFIG_MODE 0x0
#define AP_MODE 0x1

//=============================================================================
//wifi复位状态
//=============================================================================
#define RESET_WIFI_ERROR 0
#define RESET_WIFI_SUCCESS 1

//=============================================================================
//wifi模式状态
//=============================================================================
#define SET_WIFI_MODE_ERROR 0
#define SET_WIFI_MODE_SUCCESS 1

//=============================================================================
//数据帧字节顺序
//=============================================================================
#define FRAME_HEAD1 0
#define FRAME_HEAD2 1
#define PROTOCOL_VER 2
#define FUNCTION_TYPE 3
#define LENGTH_H 4
#define LENGTH_L 5
#define PAYLOAD_START 6
//=============================================================================
//数据帧类型
//=============================================================================
#define HEAT_BEAT_FUNCTION 0         //心跳包
#define HEAT_BEAT_STOP_FUNCTION 0x25 //关闭WIFI模组心跳
#define WIFI_STATE_FUNCTION 3        //wifi工作状态
#define WIFI_RESET_FUNCTION 4        //重置wifi
#define WIFI_MODE_FUNCTION 5         //选择smartconfig/AP模式
#define DATA_QUERT_FUNCTION 6        //命令下发
#define STATE_UPLOAD_FUNCTION 7      //状态上报
#define STATE_QUERY_FUNCTION 8       //状态查询

#define GET_UNIXTIME_FUNCTION 0x0c //获取Unix时间戳
#define GET_MAC_FUNCTION 0x2d      //获取模块mac
#define WIFI_TEST_FUNCTION 0x2c    //wifi功能测试

//=============================================================================
#define VERSION 0x00      //协议版本号
#define PROTOCOL_LEN 0x07 //固定协议头长度
#define PROTOCOL_HEAD1 0x55
#define PROTOCOL_HEAD2 0xaa
//=============================================================================

extern SYSTEM_XDATA volatile unsigned char wifi_uart_rx_buf[WIFI_RX_BUF_SIZE]; //串口接收缓存
extern SYSTEM_XDATA volatile unsigned char wifi_uart_tx_buf[WIFI_TX_BUF_SIZE]; //串口发送缓存
extern SYSTEM_XDATA volatile unsigned char wifi_rx_cpl;                        //接收完成一帧数据

extern SYSTEM_XDATA unsigned char reset_wifi_flag;    //重置wifi标志(TRUE:成功/FALSE:失败)
extern SYSTEM_XDATA unsigned char set_wifi_mode_flag; //设置WIFI工作模式标志(TRUE:成功/FALSE:失败)
extern SYSTEM_XDATA unsigned char wifi_status;        //wifi模块当前工作状态
extern SYSTEM_XDATA unsigned char data_syn_flag;      //数据同步
extern SYSTEM_XDATA unsigned char data_trans_flag;    //数据传输 0：传输完成  1 传输中
/*****************************************************************************
函数名称 : wifi_uart_rec_data_process
功能描述 : 串口接收数据解析
输入参数 : dat:串口收到字节数据
返回参数 : 无
使用说明 : 在MCU串口接收函数中调用该函数,并将接收到的数据作为参数传入
*****************************************************************************/
void wifi_uart_rec_data_process(unsigned char dat);

/*****************************************************************************
函数名称 : mx_uart_cmd_service
功能描述 : 串口指令处理服务
输入参数 : 无
返回参数 : 无
使用说明 : MCU主循环 while（1）周期调用处理，间隔不大于5ms
*****************************************************************************/
void mx_uart_cmd_service(void);

/*****************************************************************************
函数名称 : wifi_uart_tx_frame
功能描述 : MCU向wifi串口发送一帧数据
输入参数 : fr_type:帧类型
           len:数据长度
返回参数 : 无
*****************************************************************************/
void wifi_uart_tx_frame(unsigned char fr_type, unsigned short len);

/*****************************************************************************
函数名称 : uart_tx_buf_write_byte
功能描述 : 写wifi_uart字节
输入参数 : addr:缓存区地址;
           byte:写入字节值
返回参数 : 写入完成后的总长度
*****************************************************************************/
unsigned short uart_tx_buf_write_byte(unsigned short addr, unsigned char byte);

#ifdef WIFI_MCU_CONTROL_MODE
/*****************************************************************************
函数名称 : check_reset_wifi_flag
功能描述 : MCU获取复位wifi成功标志
输入参数 : 无
返回参数 : 复位标志:RESET_WIFI_ERROR:失败/RESET_WIFI_SUCCESS:成功
使用说明 : 1:MCU主动调用reset_wifi()后调用该函数获取复位状态
           2:如果为模块自处理模式,MCU无须调用该函数
*****************************************************************************/
unsigned char check_reset_wifi_flag(void);

/*****************************************************************************
函数名称 : reset_wifi
功能描述 : MCU主动重置wifi工作模式
输入参数 : 无
返回参数 : 无
使用说明 : 1:MCU主动调用,通过check_reset_wifi_flag()函数获取重置wifi是否成功
           2:如果为模块自处理模式,MCU无须调用该函数
*****************************************************************************/
void reset_wifi(void);

/*****************************************************************************
函数名称 : check_wifi_mode_flag
功能描述 : 获取设置wifi状态成功标志
输入参数 : 无
返回参数 : SET_WIFI_MODE_ERROR:失败/SET_WIFI_MODE_SUCCESS:成功
使用说明 : 1:MCU主动调用set_wifi_mode()后调用该函数获取复位状态
           2:如果为模块自处理模式,MCU无须调用该函数
*****************************************************************************/
unsigned char check_wifi_mode_flag(void);

/*****************************************************************************
函数名称 : set_wifi_mode
功能描述 : MCU设置wifi工作模式
输入参数 : mode:
          SMART_CONFIG:进入smartconfig模式
          AP_CONFIG:进入AP模式
返回参数 : 无
使用说明 : 1:MCU主动调用
           2:成功后,可判断set_wifi_config_state是否为TRUE;TRUE表示为设置wifi工作模式成功
           3:如果为模块自处理模式,MCU无须调用该函数
*****************************************************************************/
void set_wifi_mode(unsigned char mode);

/*****************************************************************************
函数名称 : check_wifi_status
功能描述 : MCU主动获取当前wifi工作状态
输入参数 : 无
返回参数 : WIFI_WORK_SATE_E:
          SMART_CONFIG_STATE:smartconfig配置状态
          AP_STATE:AP 配置状态
          WIFI_NOT_CONNECTED:WIFI 配置成功但未连上路由器
          WIFI_CONNECTED:WIFI 配置成功且连上路由器
          WIFI_CONN_CLOUD:WIFI 已经连接上云服务器
          WIFI_LOW_POWER:WIFI 处于低功耗模式
使用说明 : 无
*****************************************************************************/
unsigned char check_wifi_status(void);

#endif

#endif