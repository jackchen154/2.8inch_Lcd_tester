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
uchar qu_dong_qi=1;//驱动器类型，0为一般，1为风得控


uchar com2_data[5]={0};
uchar b=0;
uchar data_buf[100]={0},start_recive=0,start_fram=0,frame_len=0,data_len=0;
uchar data_buf1[20]={0},start_recive1=0,start_fram1=0,frame_len1=0,data_len1=0;

unsigned short real_data[50]={0};//主控板接收解析存放地
unsigned short real_data1[1]={0};//LCD模块接收解析存放缓存(只解析一个寄存器内容)

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

void delayms(unsigned int ms)
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

unsigned short get_crc2(uchar *ptr,uchar len)
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

//(fd0,05,05,20,180)
void motor_contrl_send(uchar device_n,uchar ctrl_mode,uchar speed,unsigned int weizhi)//舵机控制
{
      unsigned int crc;
      uchar read_buf[30]={0xaa,0x55,0xcc};
	  read_buf[3]=device_n;
	  read_buf[4]=0x10;

	  read_buf[5]=0x00;
	  read_buf[6]=0x00;

	  read_buf[7]=0x00;
	  read_buf[8]=0x07;

      read_buf[9]=0x0e;

      read_buf[10]=0x00;
      read_buf[11]=ctrl_mode;

      read_buf[12]=0x00;
      read_buf[13]=speed;

      read_buf[14]=weizhi>>8;
      read_buf[15]=weizhi&0x00ff;

      read_buf[16]=0x12;
      read_buf[17]=0x0c;

      read_buf[18]=0x00;
      read_buf[19]=0x46;

      read_buf[20]=0x00;
      read_buf[21]=0x07;

      read_buf[22]=0x00;
      read_buf[23]=0x00;

	  crc=get_crc2(&read_buf[3],21);
	  read_buf[24]=crc&0xff;
	  read_buf[25]=(crc>>8)&0xff;

	  UART1_Send(read_buf,26);

	
}

void motor_contrl_send1(uchar reg_addr ,uchar ctrl_mode,uchar speed,unsigned int weizhi)//舵机控制
{
      unsigned int crc;
      uchar read_buf[18]={0xaa,0x55,0xcc};
	  read_buf[3]=0x82;
	  read_buf[4]=0x10;

	  read_buf[5]=0x00;
	  read_buf[6]=reg_addr;

	  read_buf[7]=0x00;
	  read_buf[8]=0x03;

      read_buf[9]=0x06;

      read_buf[10]=0x00;
      read_buf[11]=ctrl_mode;

      read_buf[12]=0x00;
      read_buf[13]=speed;

      read_buf[14]=weizhi>>8;
      read_buf[15]=weizhi&0x00ff;


      read_buf[16]=0x00;
      read_buf[17]=0x00;


	  crc=get_crc2(&read_buf[3],13);
	  read_buf[16]=crc&0xff;
	  read_buf[17]=(crc>>8)&0xff;

	  UART1_Send(read_buf,18);

	
}

//限位值发送
void limit_value_contrl_send(uchar device_n ,uchar limit_up_location,uchar limit_down_location)
{
    unsigned int crc;
    uchar read_buf[30]={0xaa,0x55,0xcc};
	  read_buf[3]=device_n;
	  read_buf[4]=0x10;

	  read_buf[5]=0x00;
	  read_buf[6]=0x0b;

	  read_buf[7]=0x00;
	  read_buf[8]=0x02;

      read_buf[9]=0x04;

      read_buf[10]=0x00;
      read_buf[11]=limit_up_location;

      read_buf[12]=0x00;
      read_buf[13]=limit_down_location;
		
	  crc=get_crc2(&read_buf[3],11);
	  read_buf[14]=crc&0xff;
	  read_buf[15]=(crc>>8)&0xff;

	  UART1_Send(read_buf,16);
	
}


void biao_qing_contrl_send(unsigned int page)//表情控制发送
{
//A5 5A 04 80 03 00 C2 
    uchar read_buf[7]={0xa5,0x5a,0x04,0x80,0x03};
    read_buf[5]=page>>8;
    read_buf[6]=page&0x00ff;

	  UART1_Send(read_buf,7);
	
}

void main_board_single_write(uchar reg,unsigned short reg_data)//主控板单写
{

    unsigned int crc;
    uchar read_buf[14]={0xaa,0x55,0xcc};
	  read_buf[3]=0x81;
	  read_buf[4]=0x10;

	  read_buf[5]=0x00;
	  read_buf[6]=reg;

	  read_buf[7]=0x00;
	  read_buf[8]=0x01;

      read_buf[9]=0x02;

      read_buf[10]=reg_data>>8;//reg
      read_buf[11]=reg_data&0x00ff;


			
			read_buf[12]=0x00;
      read_buf[13]=0x00;
			
	  crc=get_crc2(&read_buf[3],9);
	  read_buf[12]=crc&0xff;
	  read_buf[13]=(crc>>8)&0xff;
	  UART1_Send(read_buf,14);
	
}
	

//限制电流发送
void limit_current_contrl_send(uchar device_n ,uchar limit_current)
{
    unsigned int crc;
    uchar read_buf[30]={0xaa,0x55,0xcc};
	  read_buf[3]=device_n;
	  read_buf[4]=0x10;

	  read_buf[5]=0x00;
	  read_buf[6]=0x07;

	  read_buf[7]=0x00;
	  read_buf[8]=0x01;

      read_buf[9]=0x02;

      read_buf[10]=0x00;
      read_buf[11]=limit_current;

		
	  crc=get_crc2(&read_buf[3],9);
	  read_buf[12]=crc&0xff;
	  read_buf[13]=(crc>>8)&0xff;

	  UART1_Send(read_buf,14);
	
}

void led_contrl_send(uchar device_n,uchar led1_cycle,uchar led2_cycle,uchar liangdu)//LED控制
{
      unsigned int crc;
      uchar read_buf[17]={0xaa,0x55,0xcc};
	  read_buf[3]=device_n;
	  read_buf[4]=0x10;

	  read_buf[5]=0x00;
	  read_buf[6]=0x00;

	  read_buf[7]=0x00;
	  read_buf[8]=0x03;

      read_buf[9]=0x06;

      read_buf[10]=0x00;
      read_buf[11]=led1_cycle;

      read_buf[12]=0x00;
      read_buf[13]=led2_cycle;

			read_buf[14]=0x00;
      read_buf[15]=liangdu;

	  crc=get_crc2(&read_buf[3],13);
	  read_buf[16]=crc&0xff;
	  read_buf[17]=(crc>>8)&0xff;
	  UART1_Send(read_buf,18);
	
}

void led_contrl_send1(uchar power,uchar red,uchar green,uchar blue)//LED控制
{
      unsigned int crc;
      uchar read_buf[20]={0xaa,0x55,0xcc};
	  read_buf[3]=0x82;
	  read_buf[4]=0x10;

	  read_buf[5]=0x00;
	  read_buf[6]=0x00;

	  read_buf[7]=0x00;
	  read_buf[8]=0x04;

      read_buf[9]=0x08;

      read_buf[10]=0x00;
      read_buf[11]=power;

      read_buf[12]=0x00;
      read_buf[13]=red;

			read_buf[14]=0x00;
      read_buf[15]=green;
			
			read_buf[16]=0x00;
      read_buf[17]=blue;			

			read_buf[18]=0x00;
      read_buf[19]=0x00;
	  crc=get_crc2(&read_buf[3],15);
	  read_buf[18]=crc&0xff;
	  read_buf[19]=(crc>>8)&0xff;
	  UART1_Send(read_buf,20);
	
}

void main_board_contrl_send(uchar run_mode,uchar left_speed,uchar right_speed)//主控板多写
{
      unsigned int crc;
      uchar read_buf[20]={0xaa,0x55,0xcc};
	  read_buf[3]=0x81;
	  read_buf[4]=0x10;

	  read_buf[5]=0x00;
	  read_buf[6]=0x00;

	  read_buf[7]=0x00;
	  read_buf[8]=0x04;

      read_buf[9]=0x08;

      read_buf[10]=0x00;
      read_buf[11]=run_mode;

      read_buf[12]=left_speed;
      read_buf[13]=right_speed;

			read_buf[14]=0x05;
      read_buf[15]=0x0a;

			read_buf[16]=0x05;
      read_buf[17]=0x0a;
			
			read_buf[18]=0x00;
      read_buf[19]=0x00;
			
	  crc=get_crc2(&read_buf[3],15);
	  read_buf[18]=crc&0xff;
	  read_buf[19]=(crc>>8)&0xff;
	  UART1_Send(read_buf,20);
	
}

void main_board_contrl_send1(uchar run_mode,unsigned short left_speed,unsigned short right_speed)//主控板多写
{
    unsigned int crc;
    uchar read_buf[23]={0xaa,0x55,0xcc};
	  read_buf[3]=0x81;
	  read_buf[4]=0x10;
	  read_buf[5]=0x00;
		
	  read_buf[6]=0x00;//起始地址

	  read_buf[7]=0x00;
	  read_buf[8]=0x03;

      read_buf[9]=0x06;

      read_buf[10]=0x00;//reg:0
      read_buf[11]=run_mode;


		
   		read_buf[12]=left_speed>>8;//reg:1
      read_buf[13]=left_speed&0x00ff;

   		read_buf[14]=right_speed>>8;//reg:2
      read_buf[15]=right_speed&0x00ff;		
			
			read_buf[16]=0x00;
      read_buf[17]=0x00;
			
	  crc=get_crc2(&read_buf[3],13);
	  read_buf[16]=crc&0xff;
	  read_buf[17]=(crc>>8)&0xff;
	  UART1_Send(read_buf,18);
	
}

void Main(void)  
{  
  UART1_Init();
  UART2_Init();
	//TH1 = 0xFD;		
  //TL1 = TH1;	//115200						
  Lcd_control("page index"); 
  //Lcd_control("page ultrasonic1"); 
  lcd_status=	Sensor_window1;
  while(1)
  { 
          if(lcd_status==Sensor_window)
          {   						
              Lcd_control("page ultrasonic");						
              while (1) 
  						{ 
                if(ultrasonic_window(real_data)<0) break;
                delayms(50);
              }
           }	
          if(lcd_status==Sensor_window1)
          {   						
              Lcd_control("page ultrasonic1");
						  main_board_single_write(0x13,0xcccc);//超声波探头盲区设置为12
						  delayms(30);
						  main_board_single_write(0x14,0xcccc);
						  delayms(30);						
						  main_board_single_write(0x15,0xcccc);
						  delayms(30);
						while (1) 
  						{ 
                if(ultrasonic_window1(real_data)<0) break;
                delayms(50);
              }
           }					
					 
					 
          if(lcd_status==Control_window)//机器人控制
          {   						
              Lcd_control("page robot control");											
              while (1) 
  						{ 
                if(robot_ctrl_window(real_data)<0) break;
                delayms(250);
              }
           }
          if(lcd_status==Control_window1)//四代机器人控制
          {   						

  						main_board_single_write(0x0e,0x7fff);//关工控串口
						  delayms(30);
						  if(qu_dong_qi==1)//如果是风得控
              main_board_single_write(0x0f,0x067f);//开安卓控制
							else
							main_board_single_write(0x0f,0x027f);//开安卓控制	
              delayms(30);						
						  Lcd_control("page robot control1");					
              while (1) 
  						{ 
								if(robot_ctrl_window1(real_data)<0) break;
                delayms(250);
              }
           }					
					 
           if(lcd_status==versions_window)
           {   
			        Lcd_control("page version");     
              while (1) 
              { 
                if(version_window(real_data)<0) break;
                delayms(250);
              }
           }

           if(lcd_status==versions_window1)
           {   

			        Lcd_control("page version1");     
              while (1) 
              { 
                if(version_window1(real_data)<0) break;
                delayms(250);
              }
           }	

					 
           if(lcd_status==Headctr_window)
           {   
              Lcd_control("page head contrl");
              TH1 = 0xf7;	
	            TL1 = TH1;	//38400波特率 						 
              motor_contrl_send(0x01,0x00,0,0);//初始化舵机
              delayms(30);
              motor_contrl_send(0x06,0x00,0,0);//初始化舵机
              delayms(30);  
              motor_contrl_send(0x07,0x00,0,0);//初始化舵机     
              while (1) //循环读取数据
              {         
                if(motor_ctrl_window(real_data)<0) 
								{
								  TH1 = 0xFD;		
                  TL1 = TH1;	//115200		
									break;
								}
                delayms(10);
              }
            } 
           if(lcd_status==duojixianzhi_window)
           {   
              Lcd_control("page duojixianzhi");
              TH1 = 0xf7;	
	            TL1 = TH1;	//38400波特率 						     
              while (1) //循环读取数据
              {         
                if(Duojixianzhi_window(real_data)<0) 
								{
   							  TH1 = 0xFD;		
                  TL1 = TH1;	//115200	
									break;
								}	
              }
            }
           if(lcd_status==Headctr_window1)
           {   
              Lcd_control("page head contrl1");			 
              while (1) //循环读取数据
              {         
                if(motor_ctrl_window1(real_data)<0) break;
                //delayms(100);
              }
            } 
						
           if(lcd_status==Biaoqing_window)//  表情窗口
           {   
              Lcd_control("page face_control");							     
              while (1) //循环读取数据
              {         
                if(biaoqing_window()<0) break;
								delayms(10);
              }
            }
					              						
        delayms(10);
        
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

void lcdjieshou(void) interrupt 8 using 1//接收字符及处理
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
     
           //页面窗口部件
           if(real_data1[0]==0x0000)
           {
             lcd_status=Control_window;
           }
           if(real_data1[0]==0x0005)
           {
             lcd_status=Control_window1;
           }	

					 
           if(real_data1[0]==0x0001)
           {
             lcd_status=Sensor_window;
           }
           if(real_data1[0]==0x0006)
           {
             lcd_status=Sensor_window1;
           }			

					 
           if(real_data1[0]==0x0002)
           {
             lcd_status=Headctr_window;
           }
           if(real_data1[0]==0x0007)
           {
             lcd_status=Headctr_window1;
           }					 
					 
           if(real_data1[0]==0x0003)
           {
             lcd_status=versions_window;
           }
           if(real_data1[0]==0x0008)
           {
             lcd_status=versions_window1;
           }					 
           if(real_data1[0]==0x0009)
           {
             lcd_status=Biaoqing_window;
           }						 
					 
           if(real_data1[0]==0x0004)
           {
             ////printf("versions_window\n");
             lcd_status=duojixianzhi_window;
           }					 
           if(real_data1[0]==0x00bc)
           {
             lcd_status=Return_button;
           }
	           if(real_data1[0]==0xbc01)
           {
             lcd_status=Return_button1;
           }				 
					 
           //头部窗口部件
           if(real_data1[0]==0x0300)
           {
             ////printf("hbiaoding\n");
             lcd_status=hbiaoding;
           }
           if(real_data1[0]==0x0301)
           {
             ////printf("hzuo\n");
             lcd_status=hzuo;
           } 
           if(real_data1[0]==0x0302)
           {
             ////printf("hzhong\n");
             lcd_status=hzhong;
           } 
           if(real_data1[0]==0x0303)
           {
             ////printf("hyou\n");
             lcd_status=hyou;
           }

           //左手窗口部件
           if(real_data1[0]==0x031a)
           {
             ////printf("jieshouzbiaoding\n");
             lcd_status=zbiaoding;
           }
           if(real_data1[0]==0x0310)
           {
             ////printf("jieshouzzuo\n");
             lcd_status=zzuo;
           } 
           if(real_data1[0]==0x0312)
           {
             ////printf("jieshouzzhong\n");
             lcd_status=zzhong;
           } 
           if(real_data1[0]==0x031b)
           {
             ////printf("jieshouzyou\n");
             lcd_status=zyou;
           }

           //右手窗口部件
           if(real_data1[0]==0x0320)
           {
             ////printf("ybiaoding\n");
             lcd_status=ybiaoding;
           }
           if(real_data1[0]==0x0321)
           {
             ////printf("yzuo\n");
             lcd_status=yzuo;
           } 
           if(real_data1[0]==0x0322)
           {
             ////printf("yzhong\n");
             lcd_status=yzhong;
           } 
           if(real_data1[0]==0x0323)
           {
             ////printf("yyou\n");
             lcd_status=yyou;
           }

           //LED灯板控制部件 
           if(real_data1[0]==0x03aa)
           {
             ////printf("zui0\n");
             lcd_status=led00;
           } 
           if(real_data1[0]==0x03bb)
           {
             ////printf("zui1\n");
             lcd_status=led01;
           } 
           if(real_data1[0]==0x03cc)
           {
             ////printf("yan0\n");
             lcd_status=led10;
           } 
           if(real_data1[0]==0x03dd)
           {
             ////printf("yan1\n");
             lcd_status=led11;
           }

           //连续控制部件 
           if(real_data1[0]==0x0334)
           {
             ////printf("lianxu0\n");
             lcd_status=alianxu0;
           } 
           if(real_data1[0]==0x0335)
           {
             ////printf("lianxu1\n");
             lcd_status=alianxu1;
           }
           //real_data1[0]=0x0000;
			     
					 //自动充电部件
					 if(real_data1[0]==0xc100)//关闭充电
           {
             lcd_status=auto_charge_off;
           } 
           if(real_data1[0]==0xc101)//开启充电
           {
             lcd_status=auto_charge_on;
           }
					 
					 //机器人控制行走部分
						if(real_data1[0]==0xd1d1)//上
						{	
             lcd_status=go_up;
            } 
           if(real_data1[0]==0xd2d2)//下
           {
             lcd_status=go_down;
           }
						if(real_data1[0]==0xd3d3)//左
           {
             lcd_status=go_left;
           } 
           if(real_data1[0]==0xd4d4)//右
           {
             lcd_status=go_right;
           }
           if(real_data1[0]==0xd0d0)//停
           {
             lcd_status=go_stop;
           }
				   //限位设置控件
					 if(real_data1[0]==0x0401)//停
           {
             lcd_status=set_limit_value;
           }
					 //表情屏部分
					 if(real_data1[0]==0xaaa0)
           {
             lcd_status=zhengchang;
           }	
					 if(real_data1[0]==0xaaa1)
           {
             lcd_status=aixin;
           }	
					 if(real_data1[0]==0xaaa2)
           {
             lcd_status=chongdian;
           }	
					 if(real_data1[0]==0xaaa3)
           {
             lcd_status=daohang;
           }
           //驱动器选择
					 if(real_data1[0]==0xfdfd)
           {
             lcd_status=feng_de_kong;
           }					 
					 if(real_data1[0]==0xfdf0)
           {
             lcd_status=yi_ban_qudong;
           }						 
					 return;

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