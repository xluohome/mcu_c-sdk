#include "uart.h"
#include "h\SC92F846XB_C.h"
#include "mx_uart_api.h"
#include "mx_mcu_app.h"

#if USE_UART0
bit UartSendFlag = 0; //发送中断标志位
bit UartReceiveFlag = 0; //接收中断标志位
/*****************************************************
*函数名称：void Uart0_Test(void)
*函数功能：Uart0测试
*入口参数：void
*出口参数：void
*****************************************************/
void Uart0_Test(void)
{
	Uart0_Init(12,9600);
	while(1)
	{
		SBUF = 0x55;
		while(!UartSendFlag);
		UartSendFlag = 0;
	}
}
/*****************************************************
*函数名称：void Uart0_Init(uint8_t8_t Freq,unsigned long int baud)
*函数功能：Uart0中断初始化
*入口参数：Freq-主频，baud-波特率
*出口参数：Freq,baud
*****************************************************/
void Uart0_Init(uint8_t Freq,unsigned long int baud)    //选择Timer1作为波特率信号发生器
{
	P1CON &= 0xF3;   //TX设置为强推挽输出高，RX设置为输入带上拉
	P1CON |= 0x08;
	P1PH  |= 0x04;
	P13 = 1;
	
	SCON  |= 0X50;   //设置通信方式为模式一，允许接收
	TMCON |= 0X02;
	TMOD  |= 0X20;
	PCON  |= 0X80;	//SMOD=1
	TH1 = (Freq*1000000/baud)>>8;	  //波特率为T1的溢出时间；
	TL1 = Freq*1000000/baud;
	TR1 = 0;
	ET1 = 0;
	EUART = 1;     //开启Uart0中断
	EA = 1;
}


/*****************************************************
*函数名称：void UartInt(void) interrupt 4
*函数功能：Uart0中断函数
*入口参数：void
*出口参数：void
*****************************************************/
void UartInt(void) interrupt 4
{
	if(TI)
	{
		TI = 0;	
		UartSendFlag = 1;		
	}
	if(RI)
	{
		RI = 0;	
		UartReceiveFlag = 1;
	}	
}

#endif


#if USE_UART1
bit Uart1SendFlag = 0;    //Uart发送中断标志位
bit Uart1ReceiveFlag = 0; //Uart接收中断标志位
bit SPIFlag = 0;          //SPI数据传输完成标志位
bit TWIFlag = 0;          //中断标志位

#if (SSI_Mode == Uart1)
static volatile uint8_t * gp_uart1_tx_address;       
static volatile uint8_t  g_uart1_tx_count; 
#endif

/*****************************************************
*函数名称：void Uart1_Init(uint8_t Freq,unsigned long int baud)
*函数功能：Uart1中断初始化
*入口参数：Freq-主频，baud-波特率
*出口参数：Freq,baud
*****************************************************/
#if (SSI_Mode == Uart1)
void Uart1_Init(uint8_t Freq,unsigned long int baud)
{
	P2CON &= 0xFC; //TX，RX设置为输入带上拉
	P2PH |= 0x03;

	OTCON = 0xC0;						   //串行接口SSI选择Uart1通信
	SSCON0 = 0x50;						   //设置通信方式为模式一，允许接收
	SSCON1 = Freq * 1000000 / baud;		   //波特率低位控制
	SSCON2 = (Freq * 1000000 / baud) >> 8; //波特率高位控制
	IE1 |= 0x01;						   //开启SSI中断
	EA = 1;
}

uint8_t uart1_send_data(uint8_t * const uart_tx_buf, uint8_t tx_num)
{	
	if (tx_num < 1U || Uart1SendFlag)
	{
		 return (0);
	}
	else
	{
		gp_uart1_tx_address = uart_tx_buf;
		g_uart1_tx_count = tx_num;		
		IE1 &= 0xfe;      //关闭SSI中断
		SSDAT = *gp_uart1_tx_address;
		gp_uart1_tx_address++;
		g_uart1_tx_count--;
        Uart1SendFlag = 1;
        data_trans_flag = 1;
		IE1 |= 0x01;      //开启SSI中断
	}

	return (0);
}
#endif
/*****************************************************
*函数名称：void TWI_Init(void)
*函数功能：TWI初始化
*入口参数：void
*出口参数：void
*****************************************************/
#if (SSI_Mode == TWI)
void TWI_Init(void)
{
	OTCON = 0x80;  //选择TWI模式
	SSCON0 = 0x80;  // ---- x---  0为不允许接收，1为允许接收
	SSCON1 = 0x01;  //xxxx xxxy  x为地址寄存器，y为0禁止/1允许通用地址响应
	IE1 = 0x01;
	EA = 1;
}
#endif
/*****************************************************
*函数名称：void TWI_Init(void)
*函数功能：TWI初始化
*入口参数：void
*出口参数：void
*****************************************************/
#if (SSI_Mode == SPI)
void SPI_Init(void)
{
	OTCON = 0X40;  //选择SPI模式
	SSCON0 = 0x2F; //设置SPI为主设备，SCK空闲时间为低电平，SCK周期第二沿采集数据，时钟速率为Fsys/512
	SSCON1 = 0x01;   //允许发送中断
	SSCON0 |= 0x80; //开启SPI
	IE1 = 0x01;
	EA = 1;
}
#endif

/*****************************************************
*函数名称：void TWI_Int() interrupt 7
*函数功能：SSI中断函数
*入口参数：void
*出口参数：void
*****************************************************/
#if (SSI_Mode == Uart1)
void Uart1_Int() interrupt 7   //Uart1中断函数
{
	if(SSCON0&0x02)    //发送标志位判断
	{
		SSCON0 &= 0xFD;
		if (g_uart1_tx_count > 0U)
		{
			SSDAT = *gp_uart1_tx_address;
			gp_uart1_tx_address++;
			g_uart1_tx_count--;
		}
		else 
		{
			Uart1SendFlag = 0;
            data_trans_flag = 0;
		}
	}
	if((SSCON0&0x01))  //接收标志位判断
	{
		SSCON0 &= 0xFE;
		Uart1ReceiveFlag = 1;
        wifi_uart_rec_data_process(SSDAT);
	}	
}
#endif

#if (SSI_Mode == TWI)
void TWI_Int() interrupt 7     //TWI中断函数
{
	if(SSCON0&0x40)
	{
		SSCON0 &= 0xbf;  //中断清零
		TWIFlag = 1;
	}	
}
#endif 

#if (SSI_Mode == SPI)
void SpiInt(void) interrupt 7    //SPI中断函数
{	  
	if(SSCON1&0X08)    //发送缓存器空标志判断
	{
		SSCON1 &= ~0X08;
	}
	if(SSCON1&0X80)    //数据传输标志位判断
	{
		SSCON1 &= ~0X80;
		SPIFlag = 1;
	}
}
#endif 

#endif