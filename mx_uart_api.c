/*
 * MCU C-SDK Ver:1.0.0
 * ©2020  MICHIP Technology inc.
 * https://github.com/xluohome/mcu_c-sdk
 */

/************************请勿修改以下代码*****************************/

#include "mx_uart_api.h"
#include <string.h>

SYSTEM_XDATA volatile unsigned char wifi_uart_rx_buf[WIFI_RX_BUF_SIZE]; //串口接收缓存
SYSTEM_XDATA volatile unsigned char wifi_uart_tx_buf[WIFI_TX_BUF_SIZE]; //串口发送缓存
SYSTEM_XDATA volatile unsigned char wifi_rx_cpl = 0;                    //接收完成一帧数据
SYSTEM_XDATA unsigned char reset_wifi_flag;                             //重置wifi标志(TRUE:成功/FALSE:失败)
SYSTEM_XDATA unsigned char set_wifi_mode_flag;                          //设置WIFI工作模式标志(TRUE:成功/FALSE:失败)
SYSTEM_XDATA unsigned char wifi_status;                                 //wifi模块当前工作状态
SYSTEM_XDATA unsigned char data_syn_flag = 0;                           //数据同步 0：不需要上传  1：需要数据上传
SYSTEM_XDATA unsigned char data_trans_flag = 0;                         //数据传输 0：传输完成  1 传输中

/*****************************************************************************
函数名称 : calc_check_sum
功能描述 : 计算校验和
输入参数 : ptr:数据源指针
           len:计算校验和长度
返回参数 : 校验和
*****************************************************************************/
static unsigned char calc_check_sum(unsigned char *ptr, unsigned short len)
{
    unsigned char check_sum = 0;
    while (len-- != 0)
    {
        check_sum += *ptr++;
    }
    return check_sum;
}

/*****************************************************************************
函数名称 : uart_tx_buf_write_byte
功能描述 : 写wifi_uart字节
输入参数 : addr:缓存区地址;
           byte:写入字节值
返回参数 : 写入完成后的总长度
*****************************************************************************/
unsigned short uart_tx_buf_write_byte(unsigned short addr, unsigned char byte)
{
    unsigned char *destination = (unsigned char *)wifi_uart_tx_buf + PAYLOAD_START + addr;

    *destination = byte;
    addr += 1;

    return addr;
}

/*****************************************************************************
函数名称 : wifi_uart_tx_frame
功能描述 : MCU向wifi串口发送一帧数据
输入参数 : fr_typ:帧类型
           len:数据长度
返回参数 : 无
*****************************************************************************/
void wifi_uart_tx_frame(unsigned char fr_typ, unsigned short len)
{
    unsigned char check_sum = 0;

    wifi_uart_tx_buf[FRAME_HEAD1] = 0x55;
    wifi_uart_tx_buf[FRAME_HEAD2] = 0xaa;
    wifi_uart_tx_buf[PROTOCOL_VER] = 0x01;
    wifi_uart_tx_buf[FUNCTION_TYPE] = fr_typ;
    wifi_uart_tx_buf[LENGTH_H] = len >> 8;
    wifi_uart_tx_buf[LENGTH_L] = len & 0xff;

    len += PROTOCOL_LEN;
    check_sum = calc_check_sum((unsigned char *)wifi_uart_tx_buf, len - 1);
    wifi_uart_tx_buf[len - 1] = check_sum;

    wifi_uart_tx_data((unsigned char *)wifi_uart_tx_buf, len);
}

/*****************************************************************************
函数名称 : heat_beat_reply
功能描述 : 心跳包检测回复
输入参数 : 无
返回参数 : 无
*****************************************************************************/
static void heat_beat_reply(void)
{
    unsigned char length = 0;
    static unsigned char mcu_power_on = FALSE;

    if (mcu_power_on == FALSE)
    {
        length = uart_tx_buf_write_byte(length, FALSE);
        mcu_power_on = TRUE;
    }
    else
    {
        length = uart_tx_buf_write_byte(length, TRUE);
    }

    wifi_uart_tx_frame(HEAT_BEAT_FUNCTION, length);
}

#ifdef WIFI_MCU_CONTROL_MODE
/*****************************************************************************
函数名称 : check_reset_wifi_flag
功能描述 : MCU获取复位wifi模块的成功标志
输入参数 : 无
返回参数 : 复位标志:RESET_WIFI_ERROR:失败/RESET_WIFI_SUCCESS:成功
使用说明 : 1:MCU主动调用reset_wifi()后调用该函数获取复位状态
           2:如果为模块自处理模式,MCU无须调用该函数
*****************************************************************************/
unsigned char check_reset_wifi_flag(void)
{
    return reset_wifi_flag;
}
/*****************************************************************************
函数名称 : reset_wifi
功能描述 : MCU主动重置wifi工作模式
输入参数 : 无
返回参数 : 无
使用说明 : 1:MCU主动调用,通过check_reset_wifi_flag()函数获取重置wifi是否成功
           2:如果为模块自处理模式,MCU无须调用该函数
*****************************************************************************/
void reset_wifi(void)
{
    reset_wifi_flag = RESET_WIFI_ERROR;

    wifi_uart_tx_frame(WIFI_RESET_FUNCTION, 0);
}
/*****************************************************************************
函数名称 : check_wifi_mode_flag
功能描述 : 获取设置wifi状态成功标志
输入参数 : 无
返回参数 : SET_WIFI_MODE_ERROR:失败/SET_WIFI_MODE_SUCCESS:成功
使用说明 : 1:MCU主动调用set_wifi_mode()后调用该函数获取复位状态
           2:如果为模块自处理模式,MCU无须调用该函数
*****************************************************************************/
unsigned char check_wifi_mode_flag(void)
{
    return set_wifi_mode_flag;
}
/*****************************************************************************
函数名称 : set_wifi_mode
功能描述 : MCU设置wifi模块的工作模式
输入参数 : mode:
          SMART_CONFIG:进入smartconfig模式
          AP_CONFIG:进入AP模式
返回参数 : 无
使用说明 : 1:MCU主动调用
           2:成功后,可判断set_wifi_config_state是否为TRUE;TRUE表示为设置wifi工作模式成功
           3:如果为模块自处理模式,MCU无须调用该函数
*****************************************************************************/
void set_wifi_mode(unsigned char mode)
{
    unsigned char length = 0;

    set_wifi_mode_flag = SET_WIFI_MODE_ERROR;

    length = uart_tx_buf_write_byte(length, mode);

    wifi_uart_tx_frame(WIFI_MODE_FUNCTION, length);
}
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
unsigned char check_wifi_status(void)
{
    return wifi_status;
}
#endif

typedef enum
{
    WAIT_DATA = 0,
    REC_HEAD,
    REC_VER,
    REC_CMD,
    REC_DATA_LEN_H,
    REC_DATA_LEN_L,
    REC_DATA,
    CHECK_SUM,
} _REC_STATE;

/*****************************************************************************
函数名称 : wifi_uart_rec_data_process
功能描述 : 串口接收数据解析
输入参数 : dat:串口收到字节数据
返回参数 : 无
使用说明 : 在MCU串口接收函数中调用该函数,并将接收到的数据作为参数传入
*****************************************************************************/
void wifi_uart_rec_data_process(unsigned char dat)
{
    volatile unsigned char check_sum;
    static SYSTEM_XDATA volatile unsigned short rec_data_cnt = 0;
    static SYSTEM_XDATA volatile unsigned short rec_data_len = 0;
    static SYSTEM_XDATA volatile _REC_STATE rec_state = WAIT_DATA;
    static SYSTEM_XDATA volatile unsigned char rx_buf[WIFI_RX_BUF_SIZE];
    switch (rec_state)
    {
    case WAIT_DATA:
        rec_data_cnt = 0;
        if (dat == PROTOCOL_HEAD1)
        {
            rec_state = REC_HEAD;
            rx_buf[FRAME_HEAD1] = dat;
        }
        else
        {
            rec_state = WAIT_DATA;
        }
        break;

    case REC_HEAD:
        if (dat == PROTOCOL_HEAD2)
        {
            rec_state = REC_VER;
            rx_buf[FRAME_HEAD2] = dat;
        }
        else
        {
            rec_state = WAIT_DATA;
        }
        break;

    case REC_VER:
        rx_buf[PROTOCOL_VER] = dat;
        rec_state = REC_CMD;
        break;

    case REC_CMD:
        rx_buf[FUNCTION_TYPE] = dat;
        rec_state = REC_DATA_LEN_H;
        break;

    case REC_DATA_LEN_H:
        rx_buf[LENGTH_H] = dat;
        rec_state = REC_DATA_LEN_L;
        break;

    case REC_DATA_LEN_L:
        rx_buf[LENGTH_L] = dat;
        rec_data_len = ((unsigned short)rx_buf[LENGTH_H] << 8) + rx_buf[LENGTH_L];
        if (rec_data_len > (WIFI_RX_BUF_SIZE - PROTOCOL_LEN)) //检查长度是否超过缓存
        {
            rec_state = WAIT_DATA;
        }
        else if (rec_data_len == 0)
        {
            rec_state = CHECK_SUM;
        }

        else
        {
            rec_state = REC_DATA;
        }
        break;

    case REC_DATA:
        rx_buf[rec_data_cnt + PAYLOAD_START] = dat;
        rec_data_cnt++;
        if (rec_data_cnt >= rec_data_len)
        {
            rec_state = CHECK_SUM;
        }
        break;

    case CHECK_SUM:
        check_sum = calc_check_sum(rx_buf, (rec_data_len + PAYLOAD_START));
        if (dat == check_sum)
        {
            memcpy(wifi_uart_rx_buf, rx_buf, WIFI_RX_BUF_SIZE);
            wifi_rx_cpl = 1;
        }
        memset(rx_buf, 0, WIFI_RX_BUF_SIZE);
        rec_state = WAIT_DATA;

        break;

    default:
        rec_state = WAIT_DATA;
        rec_data_cnt = 0;
        break;
    }
}

/*****************************************************************************
函数名称 : mx_uart_cmd_service
功能描述 : 串口指令处理服务
输入参数 : 无
返回参数 : 无
使用说明 : MCU主循环 while（1）周期调用处理，间隔不大于5ms
*****************************************************************************/
void mx_uart_cmd_service(void)
{
    unsigned char cmd_type;
    unsigned short data_len;
    unsigned short i;
    unsigned short payload_len;
    if (wifi_rx_cpl == 0)
    {
        return;
    }
    wifi_rx_cpl = 0;
    cmd_type = wifi_uart_rx_buf[FUNCTION_TYPE];
    switch (cmd_type)
    {
    case HEAT_BEAT_FUNCTION: //心跳包
        heat_beat_reply();
        break;

    case WIFI_STATE_FUNCTION: //wifi工作状态
        wifi_status = wifi_uart_rx_buf[PAYLOAD_START];
        wifi_uart_tx_frame(WIFI_STATE_FUNCTION, 0);
        break;

    case WIFI_RESET_FUNCTION: //重置wifi(wifi返回成功)
        reset_wifi_flag = RESET_WIFI_SUCCESS;
        break;

    case WIFI_MODE_FUNCTION: //选择smartconfig/AP模式(wifi返回成功)
        set_wifi_mode_flag = SET_WIFI_MODE_SUCCESS;
        break;

    case DATA_QUERT_FUNCTION: //功能指令下发
        payload_len = wifi_uart_rx_buf[LENGTH_H] << 8;
        payload_len += wifi_uart_rx_buf[LENGTH_L];

        for (i = 0; i < payload_len;)
        {

            if (wifi_uart_rx_buf[PAYLOAD_START + i + 1] == DATA_TYPE_BOOL || wifi_uart_rx_buf[PAYLOAD_START + i + 1] == DATA_TYPE_ENUM)
            {
                data_len = 1;
            }
            else if (wifi_uart_rx_buf[PAYLOAD_START + i + 1] == DATA_TYPE_INT || wifi_uart_rx_buf[PAYLOAD_START + i + 1] == DATA_TYPE_FLOAT)
            {
                data_len = 4;
            }
            else
            {
                break;
            }

            mx_func_cmd_handle((unsigned char *)(wifi_uart_rx_buf + PAYLOAD_START + i));

            i += (data_len + 2);
        }
        data_syn_flag = 1;
        break;
    case STATE_QUERY_FUNCTION: //状态查询
        data_syn_flag = 1;
        break;

    default:
        break;
    }
}