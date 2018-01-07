//#include"STC12C2052.h" 
#include"STC12C5A.h"
#include<intrins.h>  
#include "1602.h"
#define uchar unsigned char  
#define uint  unsigned int 
#define S2RI 0X01
#define S2TI 0X02

uchar com2_data[5]={0};
uchar b=0;
uchar data_buf[100]={0},start_recive=0,start_fram=0,frame_len=0,data_len=0;
uchar reboot_cnt=0;
unsigned short real_data[50]={0};

void reboot(uchar buf)
{
  if(buf==0x7f)//程序下载请求
  {
     reboot_cnt++;
	 if(reboot_cnt==30)
	 {
		reboot_cnt=0;
	    IAP_CONTR=0X60;//自动重启系统
	 }
   }
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


void UartInit(void)//使用定时器1模式
{
		SCON = 0x50;//串口工作方式1,允许接收（SCON = 0x40时禁止接收）
		//PCON = 0x80;//波特率(倍频)
		TMOD = 0x20;//定时器1工作方式2
		AUXR |= 0X40;//定时器1工作在1T模式，为全速,(掉为12T模式)
	    TH1 = 0xFD;	//115200	
		//TH1 = 0xf7;	//定时器初值高8位设置38400
		TL1 = TH1;	//定时器初值低8位设置
		EA = 1; //允许总中断 
		ES = 1; //允许UART串口的中断
		TR1 = 1;	//定时器启动    
}      

void UART2_init()//串口2初始化
{
  //AUXR1=0X10;//TXD2由P1.3转到P4.3口,RXD2由P1.2转到P4.2口
  S2CON=0X50;//选择工作方式1，允许接收数据
  BRT = 0xf7;//独立发生器的重装值
  EA = 1; //允许总中断	
  AUXR |= 0x14;//(1T模式),启动发生器工作,0X1c波特率加倍，如0x14为不加倍
  //AUXR |= 0X18;//(12T模式),启动发生器工作,波特率加倍，如0x10为不加倍
  IE2=0x01;//允许串口2中断 
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
	  for(i=0;i<11;i++)
	  {
	     SBUF = read_buf[i];
		 while(TI == 0);
		 TI=0;
	  }
	
}      

 void send_data_com2(uchar device_n,uchar reg_n,uchar reg_mun)
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
	  for(i=0;i<11;i++)
	  {
	     S2BUF = read_buf[i];
    	 while((S2CON&S2TI)==0);//等待数据发送完成，硬置位S2TI		 
	     S2CON&=~S2TI;	//清除置位，以便发送下一数据
	  }
	
}

//指针形式
void lcd_display(unsigned short *real_data)
{
   uchar disbuf[17]={0};
   disbuf[0]='v';
   disbuf[1]=(*(real_data)>>8)+48;
   disbuf[2]='.';
   disbuf[3]=(*(real_data)&0x00ff)+48;
   disbuf[4]='.';
   disbuf[5]=(*(real_data+1)>>8)+48;
   disbuf[6]=' ';
   disbuf[7]=' ';
   disbuf[8]=((*(real_data+1)&0x00ff)>>4)+48;
   disbuf[9]=(*(real_data+1)&0x000f)+48;
   disbuf[10]='-';
   disbuf[11]=(*(real_data+2)>>12)+48;
   disbuf[12]=((*(real_data+2)>>8)&0x000f)+48;
   disbuf[13]= '-';
   disbuf[14]=((*(real_data+2)>>4)&0x000f)+48;
   disbuf[15]=(*(real_data+2)&0x000f)+48;
   disbuf[16]= '\0';
   xiezfc(2,1,disbuf);
} 

/*数组形式
void lcd_display(unsigned short *real_data)
{
   uchar disbuf[17]={0};
   disbuf[0]='v';
   disbuf[1]=(real_data[0]>>8)+48;
   disbuf[2]='.';
   disbuf[3]=(real_data[0]&0x00ff)+48;
   disbuf[4]='.';
   disbuf[5]=(real_data[1]>>8)+48;
   disbuf[6]=' ';
   disbuf[7]=' ';
   disbuf[8]=((real_data[1]&0x00ff)>>4)+48;
   disbuf[9]=(real_data[1]&0x000f)+48;
   disbuf[10]='-';
   disbuf[11]=(real_data[2]>>12)+48;
   disbuf[12]=((real_data[2]>>8)&0x000f)+48;
   disbuf[13]= '-';
   disbuf[14]=((real_data[2]>>4)&0x000f)+48;
   disbuf[15]=(real_data[2]&0x000f)+48;
   disbuf[16]= '\0';
   xiezfc(2,1,disbuf);
} */ 

void clear_real_data()
{
  uchar i;
  for(i=0;i<50;i++)
  {
    real_data[i]=0x0000;
  }

}  



void Main(void)  
{  
  uchar device=0;
  unsigned short disply_data[3]={0};
  LE=0;
  UartInit();
  UART2_init();  
  init_1602();    
  xiezfc(1,1,"<version-tester>");
  xiezfc(2,1,"    ---jack_chen");
  delay(1000);
  while(1)  
  {  
    send_data_com2(0x05,0x78,3);

	//*电源管理板
    send_data(0x05,0x78,3);
    delay(30);
    if(real_data[0]!=0&&real_data[1]!=0&&real_data[2]!=0)
	{
	  device=1;
	  disply_data[0]=real_data[0];
	  disply_data[1]=real_data[1];
	  disply_data[2]=real_data[2];
	}
	clear_real_data();

    //舵机板
    send_data(0x01,0x78,3);
    delay(30);
    if(real_data[0]!=0&&real_data[1]!=0&&real_data[2]!=0)
	{
	  device=2;
	  disply_data[0]=real_data[0];
	  disply_data[1]=real_data[1];
	  disply_data[2]=real_data[2];
	}
	clear_real_data();

    //左手
    send_data(0x06,0x78,3);
    delay(30);
    if(real_data[0]!=0&&real_data[1]!=0&&real_data[2]!=0)
	{
	  device=3;
	  disply_data[0]=real_data[0];
	  disply_data[1]=real_data[1];
	  disply_data[2]=real_data[2];
	}
	clear_real_data();

    //右手
    send_data(0x07,0x78,3);
    delay(30);
    if(real_data[0]!=0&&real_data[1]!=0&&real_data[2]!=0)	
	{
	  device=4;
	  disply_data[0]=real_data[0];
	  disply_data[1]=real_data[1];
	  disply_data[2]=real_data[2];
	}
	clear_real_data();

    //呼吸灯板
    send_data(0x03,0x78,3);
    delay(30);
    if(real_data[0]!=0&&real_data[1]!=0&&real_data[2]!=0)	
	{
	  device=5;
	  disply_data[0]=real_data[0];
	  disply_data[1]=real_data[1];
	  disply_data[2]=real_data[2];
	}
	clear_real_data();

    //手呼吸灯板
    send_data(0x04,0x78,3);
    delay(30);
    if(real_data[0]!=0&&real_data[1]!=0&&real_data[2]!=0)	
	{
	  device=6;
	  disply_data[0]=real_data[0];
	  disply_data[1]=real_data[1];
	  disply_data[2]=real_data[2];
	}
	clear_real_data();	

    //右轮
    send_data(0x02,0x78,3);
    delay(30);
    if(real_data[0]!=0&&real_data[1]!=0&&real_data[2]!=0)	
	{
	  device=7;
	  disply_data[0]=real_data[0];
	  disply_data[1]=real_data[1];
	  disply_data[2]=real_data[2];
	}
	clear_real_data();

    //主控板
	//TH1 = 0xFA;	//波特率切换为115200	
	//TL1 = TH1;	
    send_data(0x81,0x78,3);
    delay(30);
    if(real_data[0]!=0&&real_data[1]!=0&&real_data[2]!=0)	
	{
	  device=8;
	  disply_data[0]=real_data[0];
	  disply_data[1]=real_data[1];
	  disply_data[2]=real_data[2];
	}
	//TH1 = 0xee;	//波特率切换回38400
	//TL1 = TH1;
	clear_real_data();
	
	if(device!=0)
	{
	  if(device==1)
	  {
	    xiezfc(1,1,"Power board     ");
		lcd_display(disply_data);		

	  }
	  if(device==2)
	  {
	    xiezfc(1,1,"Turn board   [1]");
		lcd_display(disply_data);
	  }
	  if(device==3)
	  {
	    xiezfc(1,1,"Left hand    [6]");
		lcd_display(disply_data);
	  }
	  if(device==4)
	  {
	    xiezfc(1,1,"Right hand   [7]");
		lcd_display(disply_data);
	  }
	  if(device==5)
	  {
	    xiezfc(1,1,"Head-LED     [3]");
		lcd_display(disply_data);
	  }
	  if(device==6)
	  {
	    xiezfc(1,1,"Hand-LED     [4]");
		lcd_display(disply_data);
	  }
	  if(device==7)
	  {
	    xiezfc(1,1,"Turn board   [2]");
		lcd_display(disply_data);
	  }
	  if(device==8)
	  {
	    xiezfc(1,1,"Main board      ");
		lcd_display(disply_data);
	  }

	  device=0;	
	  disply_data[0]=0;
	  disply_data[1]=0;
	  disply_data[2]=0;
	}
    else
    {
	   xiezfc(1,1,"Testing....     ");
	   xiezfc(2,1,com2_data);
    }       	   

    delay(300);//*/
  }
}
	
void get_data(void)	interrupt 4  
{
  uchar i,j;
  unsigned short crc_result,ck_crc;  
            
  if(RI == 1)   //当硬件接收到一个数据时，RI会置位	
  {
    RI = 0; 
    data_buf[data_len]=SBUF;
	reboot(data_buf[data_len]);
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

void UART2_jieshou(void) interrupt 8//接收字符及处理
{ 
 
	if(S2CON&S2RI)//有数据传来，硬件置位S2RI
	{
	   S2CON&=~S2RI;	//清除置位，以便下一数据传来		 
	   com2_data[b] = S2BUF;
	   b++;
	   if(b==5)
	   { com2_data[4]='\0';
	     b=0; 
	   }	 
    }
}