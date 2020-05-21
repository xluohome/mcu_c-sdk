/*
 * MCU C-SDK Ver:1.0.0
 * ©2020  MICHIP Technology inc.
 * https://github.com/xluohome/mcu_c-sdk
 */

#include "mx_mcu_app.h"
#include "mx_uart_api.h"
#include "uart.h"

/******************************************************************************
                         定义数据变量，请按照实际数据修改            
******************************************************************************/
SYSTEM_XDATA unsigned char power_switch;
SYSTEM_XDATA unsigned char fan_speed;
SYSTEM_XDATA unsigned char light;
SYSTEM_XDATA unsigned char keep_warm;
SYSTEM_XDATA unsigned char cleaning;

/*****************************************************************************
函数名称 : wifi_uart_tx_data
功能描述 : 向uart写入连续数据
输入参数 : ptr:发送缓存指针
           len:数据发送长度
返回参数 : 无
*****************************************************************************/
void wifi_uart_tx_data(unsigned char *ptr, unsigned short len)
{

#error "请将MCU uart发送函数填入后删除本行"

  if ((NULL == ptr) || (0 == len))
  {
    return;
  }

  uart1_send_data(ptr, len); //本函数实现请参考uart.c
}

/*****************************************************************************
函数名称 : mx_func_cmd_handle
功能描述 : 模块下发产品功能指令处理
输入参数 : value:下发数据源指针
返回参数 : ret:返回数据处理结果
*****************************************************************************/
unsigned char mx_func_cmd_handle(const unsigned char value[])
{

#error "请在此函数中实现产品功能指令处理逻辑后删除本行"

  /*********************************
 产品功能指令执行后会将处理结果上报至云端,否则云端会认为下发失败
  ***********************************/
  unsigned char cmd_type;
  unsigned char data_type;
  unsigned char ret;

  cmd_type = value[0];
  data_type = value[1];

  switch (cmd_type)
  {
  case 0x14: //电源开关
    power_switch = value[2];
    if (power_switch == 0) //关机
    {
      fan_speed = 0;
      keep_warm = 0;
      light = 0;
      cleaning = 0;
    }

    break;

  case 0x0f: //照明
    light = value[2];
    break;

  case 0x15: //风速
    fan_speed = value[2];
    break;

  case 0x19: //保温
    keep_warm = value[2];
    break;

  case 0x10: //清洁功能
    cleaning = value[2];
    break;
  default:
    break;
  }
  return ret;
}

/*****************************************************************************
函数名称 : all_data_sync
功能描述 : muc数据与云端同步
输入参数 : 无
返回参数 : 无
使用说明 : MCU必须实现本函数内数据上报功能;包括只上报和可上报可下发型数据
*****************************************************************************/
static void all_data_sync(void)
{
#error "请根据示例完成数据上报处理，完成后删除本行"

  //请参考接口文档新增、修改每个功能可下发可上报变量和只上报变量
  unsigned short length = 0;
  //电源开关
  length = uart_tx_buf_write_byte(length, 0x14);
  length = uart_tx_buf_write_byte(length, DATA_TYPE_BOOL);
  length = uart_tx_buf_write_byte(length, power_switch);
  //照明
  length = uart_tx_buf_write_byte(length, 0x0f);
  length = uart_tx_buf_write_byte(length, DATA_TYPE_BOOL);
  length = uart_tx_buf_write_byte(length, light);
  //风速
  length = uart_tx_buf_write_byte(length, 0x15);
  length = uart_tx_buf_write_byte(length, DATA_TYPE_ENUM);
  length = uart_tx_buf_write_byte(length, fan_speed);
  //保温
  length = uart_tx_buf_write_byte(length, 0x19);
  length = uart_tx_buf_write_byte(length, DATA_TYPE_BOOL);
  length = uart_tx_buf_write_byte(length, keep_warm);
  //清洁
  length = uart_tx_buf_write_byte(length, 0x10);
  length = uart_tx_buf_write_byte(length, DATA_TYPE_BOOL);
  length = uart_tx_buf_write_byte(length, cleaning);
  wifi_uart_tx_frame(STATE_UPLOAD_FUNCTION, length);
}

/*****************************************************************************
函数名称 : mx_mcu_data_update
功能描述 : 数据上报更新
输入参数 : 无
返回参数 : 无
使用说明 : MCU 主循环while（1）周期调用处理，间隔不大于5ms
*****************************************************************************/
void mx_mcu_data_update(void)
{
  if (data_trans_flag == 1) //数据传输中跳出
  {
    return;
  }
  if (data_syn_flag == 0) //判断数据是否需要上报
  {
    return;
  }
  data_syn_flag = 0;
  all_data_sync();
}

#ifdef WIFI_STATUS_ENABLE
/*****************************************************************************
函数名称 : wifi_status_process
功能描述 : wifi状态业务处理逻辑
输入参数 : 
           0x00:wifi状态 1 smartconfig 配置状态
           0x01:wifi状态 2 AP 配置状态
           0x02:wifi状态 3 WIFI 已配置但未连上路由器
           0x03:wifi状态 4 WIFI 已配置且连上路由器
           0x04:wifi状态 5 已连上路由器且连接到云端
           0x05:wifi状态 6 WIFI 设备处于低功耗模式
返回参数 : 无
使用说明 : MCU需要自行实现该功能
*****************************************************************************/
void wifi_status_process(void)
{
#error "请完成wifi状态业务处理逻辑并删除本行"

  unsigned short wifi_status;

  wifi_status = check_wifi_status();
  switch (wifi_status)
  {
  case 0:
    //wifi工作状态1
    break;

  case 1:
    //wifi工作状态2
    break;

  case 2:
    //wifi工作状态3
    break;

  case 3:
    //wifi工作状态4
    break;

  case 4:
    //wifi工作状态5
    break;

  case 5:
    //wifi工作状态6
    break;

  default:
    break;
  }
}
#endif