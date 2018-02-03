#include "uart.h"
#include "lcd_window.h"
#include "main.h"
#define uchar unsigned char
#define FALSE  -1
#define TRUE   0


int Lcd_control(char *cmd)//模式设置
{
  //uchar receive_buf[4];//LCD返回数据buf
  uchar *cmdbuf=(uchar *)cmd;
  uchar end_frame[]={0xff,0xff,0xff};//LCD发送帧尾部
  uchar len=0;

  while(*cmdbuf!='\0')//计算字符个数
  {
    cmdbuf++;
    len++;
  }
   
  UART2_Send((uchar *)cmd,len);//发送LCD控制指令
  
  UART2_Send(end_frame,3);//发送LCD帧尾

  return TRUE;   

}

/******************************************************************
功能：将一个32位长整型变量dat转为字符串，比如把1234转为"1234"
参数：dat:待转的long型的变量
     str:指向字符数组的指针，转换后的字节串放在其中   
返回：转换后的字符串长度                                
******************************************************************/
unsigned char int2str(int dat,unsigned char *str) // 长整型数转换为字符串，返回值为转换后的字符串长度
{
  signed char i=0;
  unsigned char len = 0;
  unsigned char buf[6]={0}; // 长整数最大值4294967295，转ASCII码后占用10+1=11字节
  if (dat < 0)// 如果为负数，首先取绝对值，并添加负号
  {
    dat = -dat;
    *str++ = '-';
    len++;
  }
  do
  {// 低位在前高位在后顺序排列
    buf[i++] = dat % 10+0x30;// C语言中数组下标固定从0开始
    dat /= 10;
  } while (dat > 0);
  len += i;                 // i最后的值就是有效字符的个数
  while (i-- > 0)           // 高位在前低位在后顺序排列
  {
    *str++ = buf[i] ;
  }     
  *str = 0;                 // 添加字符串结束符方便使用KEIL自带的字符串处理函数处理
  return len;               // 返回字符串长度
}


int Lcd_set_val(char *cmd, int val)//模式设置
{
  //uchar receive_buf[4];
  uchar val_buf[5]={0};//int转char的缓存 
  uchar val_len=0;//转换后的数字有多少位数
  uchar *cmd_buf = (uchar *)cmd;
  uchar end_frame[]={0xff,0xff,0xff};//帧尾
  uchar cmd_len=0;
  val_len = int2str(val,val_buf);//整型转字符型

  while(*cmd_buf!='\0')
  {
    cmd_buf++;
    cmd_len++;
  }

  UART2_Send((uchar *)cmd,cmd_len);//发送LCD控制指令

  UART2_Send(val_buf,val_len);//发送数字 

  UART2_Send(end_frame,3);//帧尾部
 
    return TRUE;    

}


//tou_led.txt="V2.0.2"
int Lcd_set_txt(char *cmd,unsigned short *real_data)//模式设置
{

  uchar *cmd_buf = (uchar *)cmd;
  uchar clear_data=50;
  uchar end_frame[]={0xff,0xff,0xff};//帧尾
  uchar disbuf[8]={0};
  uchar cmd_len=0;

  disbuf[0]='"';
  disbuf[1]='V';
  disbuf[2]=(*(real_data)>>8)+48;
  disbuf[3]='.';
  disbuf[4]=(*(real_data)&0x00ff)+48;
  disbuf[5]='.';
  disbuf[6]=(*(real_data+1)>>8)+48;
  disbuf[7]='"';

  while(*cmd_buf!='\0')
  {
    cmd_buf++;
    cmd_len++;
  }

  UART2_Send((uchar *)cmd,cmd_len);//发送LCD控制指令
  
  if(*(real_data)==0 && *(real_data+1)==0)
  UART2_Send((uchar *)"\"no device\"",11);
  else
  UART2_Send(disbuf,8);//发送值 
  
  UART2_Send(end_frame,3);//帧尾部
 
  while(clear_data--)
  {
    *real_data=0x0000;
    real_data++;
  }   
  return TRUE;    
  
}


int ultrasonic_window(unsigned short *real_data)// 传感器窗口数据刷新
{        
   uchar clear_data=50;                           		
   Lcd_set_val("d0.val=",(*(real_data+chaosheng1) >> 8)*10);
   Lcd_set_val("d2.val=",(*(real_data+chaosheng1) & 0x00ff)*10);
   Lcd_set_val("d4.val=",(*(real_data+chaosheng2) >>8)*10);
   Lcd_set_val("d6.val=",(*(real_data+chaosheng2) & 0x00ff)*10);
   Lcd_set_val("d8.val=",(*(real_data+chaosheng3) >> 8)*10);
   Lcd_set_val("da.val=",(*(real_data+chaosheng3) & 0x00ff)*10);
   Lcd_set_val("dc.val=",(*(real_data+chaosheng4) >> 8)*10);
   Lcd_set_val("de.val=",(*(real_data+chaosheng4) & 0x00ff)*10);
   Lcd_set_val("e0.val=",(*(real_data+chaosheng5) >> 8)*10);
   Lcd_set_val("e2.val=",(*(real_data+chaosheng5) & 0x00ff)*10);
   Lcd_set_val("e4.val=",(*(real_data+chaosheng6) >> 8)*10);
   Lcd_set_val("e8.val=",(*(real_data+chaosheng6) & 0x00ff)*10);
   Lcd_set_val("hongwaizuo.val=",(*(real_data+hongwai2) >> 8)*2);
   Lcd_set_val("hongwaiyou.val=",(*(real_data+hongwai2) & 0x00ff)*2);
	 
	 while(clear_data--)
   {
    *real_data=0x0000;
    real_data++;
   }
   if(lcd_status==Return_button)
   {
        Lcd_control("page main");
        lcd_status = main_window;
        return -1;//如果受到返回信号
   }
   return 0;//正常刷新数据
}


int version_window(unsigned short *real_data)// 传感器窗口数据刷新
{        
         
       //电源管理板
      send_data(0x05,0x78,3);
      delayms(30);
      Lcd_set_txt("dianyuanguanli.txt=",real_data);
      //舵机板
      send_data(0x01,0x78,3);
      delayms(30);
      Lcd_set_txt("tou_duoji.txt=",real_data);
      //左手
      send_data(0x06,0x78,3);
      delayms(30);
      Lcd_set_txt("zuoshou.txt=",real_data);
      //右手
      send_data(0x07,0x78,3);
      delayms(30);
      Lcd_set_txt("youshou.txt=",real_data);
      //头呼吸灯板
      send_data(0x03,0x78,3);
      delayms(30);
      Lcd_set_txt("tou_led.txt=",real_data);
      //臂部灯板
      send_data(0x04,0x78,3);
      delayms(30);
      Lcd_set_txt("bibu_led.txt=",real_data);
                             		
     if(lcd_status==Return_button)
     {
        Lcd_control("page main");
        lcd_status = main_window;
        return -1;//如果受到返回信号
     }
   
   return 0;//正常刷新数据
}

