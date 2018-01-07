//#include"STC12C5A.h"
#include <intrins.h>
#define port P0
#define uchar unsigned char
sbit D7=port^7;
sbit D6=port^6;
sbit D5=port^5;
sbit D4=port^4;
sbit EN=port^3;
sbit RW=port^2;
sbit RS=port^1;
sbit LE=port^0;

bit lcd_busy();
void delay( int ms);
void write_com(uchar com);
void write_data(uchar date);
void init_1602(); 
void xiezfc(uchar hang,uchar lie,uchar *p);//字符串
void Delay1ms();



void delay( int ms)
{                           
   while(ms--)
   {
     Delay1ms();
   }

}

void Delay1ms()		//@11.0592MHz
{
	unsigned char i, j;

	_nop_();
	i = 11;
	j = 190;
	do
	{
		while (--j);
	} while (--i);
}

void write_com(uchar com)//写指令
{
  while(lcd_busy());
  D7=com&0x80;
  D6=com&0x40;
  D5=com&0x20;
  D4=com&0x10;
  RS=0;
  RW=0;
  EN=0;
   
   
  EN=1;	        //先传输高4位
   
   
  EN=0;
  D7=com&0x08;	 //再传输低4位
  D6=com&0x04;
  D5=com&0x02;
  D4=com&0x01;
  RS=0;
  RW=0;
  EN=0;
   
   
  EN=1;		  
   
   
  EN=0;
  RS=0;
  RW=0;
  delay(5);

}


void write_data(uchar date)//写数据
{
  while(lcd_busy());
  D7=date&0x80;
  D6=date&0x40;
  D5=date&0x20;
  D4=date&0x10;
  RS=1;
  RW=0;
  EN=0;
   
   
  EN=1;		   //使能高4位
   
   
  EN=0;
  D7=date&0x08;
  D6=date&0x04;
  D5=date&0x02;
  D4=date&0x01;
  RS=1;
  RW=0;
  EN=0;
   
   
  EN=1;		   //使能低4位
   
   
  EN=0;
  delay(5);
}

bit lcd_busy() //忙碌标志位
{                          
    bit result;
    RS = 0;
    RW = 1;
    EN = 1;
     
     
     result =D7;
    EN = 0;
    return result; 
}



void xiezfc(uchar hang,uchar lie,uchar *p)
                                      //在指定的位置放入字符串*P为字符串
{							       	  //所在数组的首地址
	uchar a;
	if(hang == 1) a = 0x80;
	if(hang == 2) a = 0xc0;
	a = a + lie - 1;
	 write_com(a);
	while(1)
	{
	
	
		if(*p == '\0') break;
		write_data(*p);
		p++;
	}
}

void init_1602()
{
  delay(10);//延时一下50
  write_com(0x28);//4总线，双行显示，5x7点阵字符
  write_com(0x28);
  write_com(0x0c);//00001DCB{D:显示(1:开,0:关)
  				  //		 C:光标(1:显示,0:不显)	 
  				  //		 B:光标闪(1:闪,0:不闪)
				  //         }
  write_com(0x01);//清屏


}