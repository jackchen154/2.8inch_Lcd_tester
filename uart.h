#ifndef _UART_H
#define _UART_H
#define uchar unsigned char
void UART1_Send(uchar *send_buf,uchar data_len);
void UART2_Send(uchar *send_buf,uchar data_len);
void UART1_Init();
void UART2_Init();

#endif
