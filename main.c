#include "STC12C5A.h"
#include "main.h"
#include "lcd_window.h"
#include "uart.h"
#include <intrins.h>  
#define uchar unsigned char  
#define uint  unsigned int 
#define S2RI 0X01
#define S2TI 0X02

uchar lcd_status=0;//LCD界面切换状态

uchar com2_data[5]={0};
uchar b=0;
uchar data_buf[100]={0},start_recive=0,start_fram=0,frame_len=0,data_len=0;
uchar data_buf1[20]={0},start_recive1=0,start_fram1=0,frame_len1=0,data_len1=0;

unsigned short real_data[50]={0};
unsigned short real_data1[1]={0};

void Delay1ms()		//@11.0592MHz
{
	unsigned char i, j;

	_nop_();
	_nop_();
	_nop_();
	i = 11;
	j = 190;
	do
	{
		while (--j);
	} while (--i);
}

void delayms(uchar ms)
{
  while(ms--)
  Delay1ms();
}



void reboot(void)
{
	IAP_CONTR=0X60;//自动重启系统
}
unsigned short get_crc1(uchar *ptr,uchar len)
{
  uchar i;
  unsigned short crc=0xFFFF;
  if(len==0) len=1;
  while(len--)
  {
    crc ^=*ptr;
    for(i=0;i<8;i++)
    {
      if(crc&1)
      {
        crc>>=1;
        crc ^= 0XA001; 
      }
      else crc>>=1;
    }
    ptr++;
  }
  return(crc);
}

unsigned short get_crc(uchar *ptr,uchar len)
{
  uchar i;
  unsigned short crc=0xFFFF;
  if(len==0) len=1;
  while(len--)
  {
    crc ^=*ptr;
    for(i=0;i<8;i++)
    {
      if(crc&1)
      {
        crc>>=1;
        crc ^= 0XA001; 
      }
      else crc>>=1;
    }
    ptr++;
  }
  return(crc);
}  

unsigned short get_crc3(uchar *ptr,uchar len)
{
  uchar i;
  unsigned short crc=0xFFFF;
  if(len==0) len=1;
  while(len--)
  {
    crc ^=*ptr;
    for(i=0;i<8;i++)
    {
      if(crc&1)
      {
        crc>>=1;
        crc ^= 0XA001; 
      }
      else crc>>=1;
    }
    ptr++;
  }
  return(crc);
} 

void send_data(uchar device_n,uchar reg_n,uchar reg_mun)
{
	  unsigned short crc;
	  uchar i=0;
    uchar read_buf[11]={0xaa,0x55,0xcc};
	  read_buf[3]=device_n;
	  read_buf[4]=0x03;
	  read_buf[5]=0x00;
	  read_buf[6]=reg_n;
	  read_buf[7]=0x00;
	  read_buf[8]=reg_mun;
	  crc=get_crc1(&read_buf[3],6);
	  read_buf[9]=crc&0xff;
	  read_buf[10]=(crc>>8)&0xff;
	  UART1_Send(read_buf,11);
	
}      


void Main(void)  
{  
  UART1_Init();
  UART2_Init();

  Lcd_control("page main");     
  while(1)
  { 
          if(lcd_status==Sensor_window)
          {   
              Lcd_control("page ultrasonic");     
              while (1) 
  						{ 
                if(ultrasonic_window(real_data)<0) break;
                delayms(1000);
              }
           }

           if(lcd_status==versions_window)
           {   

			        Lcd_control("page version");     
              while (1) 
              { 
                if(version_window(real_data)<0) break;
                delayms(1000);
              }
           } 
        delayms(100);
        
   }//end while(1)
}
	
void zhukongjieshou(void)	interrupt 4  
{
  uchar i,j;
  unsigned short crc_result,ck_crc;            
  if(RI == 1)   //当硬件接收到一个数据时，RI会置位	
  {
    RI = 0; 
    data_buf[data_len]=SBUF;
    if(start_recive==0)
    {
      if(data_len==0&&data_buf[data_len]==0xaa)
      {  
        data_len=1; 
      }      
	  else if(data_len==1&&data_buf[data_len]==0xaa)//如下次传来的还是帧头再次以aa开始判断帧
      {  
       data_len=1;  
      } 
      else if(data_len==1&&data_buf[data_len]==0x55)
      {
        data_len=2;  
      }    
      else if(data_len==2&&data_buf[data_len]==0xcc)
      {  
       start_recive=1;
      }	
      else 
      {
       data_len=0;
      }
    }    
    if(start_recive==1)
    {
      if(start_fram==1)//接收长度限制
      {
        if(frame_len>=70)
        {
           start_recive=0;           
           start_fram=0;
		   data_len=0;
		   frame_len=0;
           return;
        }
        frame_len--;
        if(frame_len==0)
        {
           crc_result = get_crc(&data_buf[3],data_buf[5]+3);//数据帧计算CRC
           ck_crc = data_buf[ (data_buf[5]+7)]<<8 | data_buf[ (data_buf[5]+6) ];//低高字节CRC转换为整数
	       if(crc_result==ck_crc)//CRC校验比对
	       {  
	         for(i=0;i<(data_buf[5]/2);i++)//计算数据对个数
	         {
	           real_data[i]=data_buf[6+j]<<8| data_buf[7+j];//有效数据合为2个字节的数据（一个寄存器的数据值为2个字节） 
	           j=j+2;
	         }	         
         }
				 if(real_data[0]==0x1122 && real_data[1]==0x3344)
				 reboot();
         start_recive=0;
         start_fram=0;
		     data_len=0;
		     frame_len=0;
		     j=0; i=0;
         return;
        }
      }        
      if(data_len==5)
      {
        start_fram=1;
        frame_len=data_buf[5]+2;
      }
      data_len++;
    }
  }
}

void lcdjieshou(void) interrupt 8//接收字符及处理
{ 

 uchar i,j;
 unsigned short crc_result,ck_crc;

if(S2CON&S2RI)//有数据传来，硬件置位S2RI
{
 S2CON&=~S2RI;	//清除置位，以便下一数据传来		 
   data_buf1[data_len1]=S2BUF; 
     if(start_recive1==0)
     {
       if(data_len1==0&&data_buf1[data_len1]==0xaa)
       {  
         data_len1=1;         
       }      
	   else if(data_len1==1&&data_buf1[data_len1]==0xaa)//如下次传来的还是帧头再次以aa开始判断帧
       {  
        data_len1=1;  
       } 
       else if(data_len1==1&&data_buf1[data_len1]==0x55)
       {
         data_len1=2;           
       }    
       else if(data_len1==2&&data_buf1[data_len1]==0xcc)
       {  
        start_recive1=1;        
       }	
       else 
       {
        data_len1=0;
       }
     }    
     if(start_recive1==1)
     {
        
       if(start_fram1==1)//接收长度限制
       {
        if(frame_len1>=70)
        {
           start_recive1=0;           
           start_fram1=0;
		   data_len1=0;
		   frame_len1=0;
        }
        frame_len1--;
        if(frame_len1==0)
        {
           crc_result = get_crc3(&data_buf1[3],data_buf1[5]+3);//数据帧计算CRC
           ck_crc = data_buf1[ (data_buf1[5]+7)]<<8 | data_buf1[ (data_buf1[5]+6) ];//低高字节CRC转换为整敿
	       if(crc_result==ck_crc)//CRC校验比对
	       {  
	         for(i=0;i<(data_buf1[5]/2);i++)//计算数据对个敿
	         {
	           real_data1[i]=data_buf1[6+j]<<8| data_buf1[7+j];//有效数据合为2个字节的数据（一个寄存器的数据值为2个字节） 
	           j=j+2;
	         }	         
           }            
           start_recive1=0;
           start_fram1=0;
		   data_len1=0;
		   frame_len1=0;
		   j=0; i=0;

           if(real_data1[0]==0x0000)
           {
             //printf("Control_window\n");
             lcd_status=Control_window;
           }
           if(real_data1[0]==0x0001)
           {
             //printf("Sensor_window\n");
             lcd_status=Sensor_window;
           }
           if(real_data1[0]==0x0002)
           {
             //printf("Headctr_window\n");
             lcd_status=Headctr_window;
           }
           if(real_data1[0]==0x0003)
           {
             //printf("versions_window\n");
             lcd_status=versions_window;
           } 
           if(real_data1[0]==0x00bc)
           {
             //printf("back button\n");
             lcd_status=Return_button;
           }          

        }
      }        
      if(data_len1==5)
      {
        start_fram1=1;
        frame_len1=data_buf1[5]+2;
      }
      data_len1++;
    }
   
 }
}