#include"STC12C5A.h"
#include "uart.h"
#define S2RI 0X01
#define S2TI 0X02

void UART1_Send(uchar *send_buf,uchar data_len)
{
  while(data_len--)
  {
	 SBUF = *send_buf;
   while(TI == 0);
	 TI=0;
	 send_buf++;
  } 
}

void UART2_Send(uchar *send_buf,uchar data_len)
{
  
  while(data_len--)
	{
	 S2BUF = *send_buf; 
   while(( S2CON & S2TI )==0);//等待数据发送完成，硬置位S2TI		 
	 S2CON &= ~S2TI;	//清除置位，以便发送下一数据
	 send_buf++;
  }
   
}

 

void UART1_Init()
{
		SCON = 0x50;//串口工作方式1,允许接收（SCON = 0x40时禁止接收）
		//PCON = 0x80;//波特率(倍频)
		TMOD = 0x20;//定时器1工作方式2
		AUXR |= 0X40;//定时器1工作在1T模式，为全速,(掉为12T模式)
	  //TH1 = 0xDC;	//9600 
		//TH1 = 0xFD;	//115200	
		TH1 = 0xf7;	//定时器初值高8位设置38400
		TL1 = TH1;	//定时器初值低8位设置
		EA = 1; //允许总中断 
		ES = 1; //允许UART串口的中断
		TR1 = 1;	//定时器启动   

}

void UART2_Init()
{
  //AUXR1=0X10;//TXD2由P1.3转到P4.3口,RXD2由P1.2转到P4.2口
  S2CON=0X50;//选择工作方式1，允许接收数据
  //BRT = 0xf7;//38400
	BRT = 0xdc;//9600
  //BRT = 0xFd;//115200
  EA = 1; //允许总中断	
  AUXR |= 0x14;//(1T模式),启动发生器工作,0X1c波特率加倍，如0x14为不加倍
  //AUXR |= 0X18;//(12T模式),启动发生器工作,波特率加倍，如0x10为不加倍
  IE2=0x01;//允许串口2中断 

}

