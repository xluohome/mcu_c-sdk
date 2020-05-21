
#ifndef __UART_H
#define __UART_H

#include "macro.h"

#define USE_UART0	0
#define USE_UART1	1
//选择三合一模式：
#define  Uart1  0
#define  TWI    1
#define  SPI    2

#define  SSI_Mode  Uart1

#if USE_UART1
static uint8_t uart1_send_data(uint8_t * const uart_tx_buf, uint8_t tx_num);
void Uart1_Init(uint8_t Freq,unsigned long int baud);
#endif

void Uart0_Init(uint8_t Freq,unsigned long int baud);

void TWI_Init(void);
void SPI_Init(void);
void SSI_Test(void);
#endif 