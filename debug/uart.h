#ifndef _UART_H
#define _UART_H
#define uchar unsigned char
int UART_Open(int fd,const char* port);

void UART_Close(int fd);

int UART_Init(int fd,int flow_ctrl,int speed,int parity,int databits,int stopbits);

int UART_Recv(int fd, uchar *rcv_buf,int data_len);

int UART_Send(int fd, uchar *send_buf,int data_len);

void UART_clear_buf(int fd);

#endif
