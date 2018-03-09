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
   while(( S2CON & S2TI )==0);//�ȴ����ݷ�����ɣ�Ӳ��λS2TI		 
	 S2CON &= ~S2TI;	//�����λ���Ա㷢����һ����
	 send_buf++;
  }
   
}

 

void UART1_Init()
{
		SCON = 0x50;//���ڹ�����ʽ1,������գ�SCON = 0x40ʱ��ֹ���գ�
		//PCON = 0x80;//������(��Ƶ)
		TMOD = 0x20;//��ʱ��1������ʽ2
		AUXR |= 0X40;//��ʱ��1������1Tģʽ��Ϊȫ��,(��Ϊ12Tģʽ)
	  //TH1 = 0xDC;	//9600 
		//TH1 = 0xFD;	//115200	
		TH1 = 0xf7;	//��ʱ����ֵ��8λ����38400
		TL1 = TH1;	//��ʱ����ֵ��8λ����
		EA = 1; //�������ж� 
		ES = 1; //����UART���ڵ��ж�
		TR1 = 1;	//��ʱ������   

}

void UART2_Init()
{
  //AUXR1=0X10;//TXD2��P1.3ת��P4.3��,RXD2��P1.2ת��P4.2��
  S2CON=0X50;//ѡ������ʽ1�������������
  //BRT = 0xf7;//38400
	BRT = 0xdc;//9600
  //BRT = 0xFd;//115200
  EA = 1; //�������ж�	
  AUXR |= 0x14;//(1Tģʽ),��������������,0X1c�����ʼӱ�����0x14Ϊ���ӱ�
  //AUXR |= 0X18;//(12Tģʽ),��������������,�����ʼӱ�����0x10Ϊ���ӱ�
  IE2=0x01;//������2�ж� 

}

