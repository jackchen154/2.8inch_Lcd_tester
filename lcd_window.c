#include "STC12C5A.h"
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


	 send_data(0x81,0x80,27); 
	 delayms(30);
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
       TH1 = 0xf7;	
	     TL1 = TH1;	//38400波特率 
			 delayms(30);
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
      delayms(30);
			
		  TH1 = 0xFD;		
		  TL1 = TH1;	//115200波特率	
			//主控板
			send_data(0x81,0x78,3);
      delayms(30);
      Lcd_set_txt("zhukong.txt=",real_data);
			//超宽带
			send_data(0x81,0xB1,3);
      delayms(30);
      Lcd_set_txt("chaokuandai.txt=",real_data);			
			//左轮
			send_data(0x81,0xB4,3);
      delayms(30);
      Lcd_set_txt("zuolun.txt=",real_data);	   
			//右轮
			send_data(0x81,0xB7,3);
      delayms(30);
      Lcd_set_txt("youlun.txt=",real_data);
			
     if(lcd_status==Return_button)
     {
        Lcd_control("page main");
        lcd_status = main_window;
        return -1;//如果受到返回信号
     }
   
   return 0;//正常刷新数据
}


int motor_ctrl_window(unsigned short *real_data)//舵机控制窗口
{        
         

      send_data(0x01,0x7e,2);
      delayms(30);
      Lcd_set_val("hbianmaqiz.val=", (int)(*(real_data)));
      delayms(30);
      Lcd_set_val("hbiaodingz.val=", (int)(*(real_data+1)));
      *(real_data)=0;
      *(real_data+1)=0;

      send_data(0x06,0x7e,2);
      delayms(30);
      Lcd_set_val("zbianmaqiz.val=", (int)(*(real_data)));
      delayms(30);
      Lcd_set_val("zbiaodingz.val=", (int)(*(real_data+1)));
      *(real_data)=0;
      *(real_data+1)=0;

      send_data(0x07,0x7e,2);
      delayms(30);
      Lcd_set_val("ybianmaqiz.val=", (int)(*(real_data)));
      delayms(30);
      Lcd_set_val("ybiaodingz.val=", (int)(*(real_data+1)));
      *(real_data)=0;
      *(real_data+1)=0;
      //*头部控制部分
      if(lcd_status==hbiaoding)//头部标定检测
      {
        motor_contrl_send(0x01,0x05,0,0);
        delayms(10);
        motor_contrl_send(0x01,0x05,0,0);
        delayms(10);         
        motor_contrl_send(0x01,0x00,0,0);
      }
      if(lcd_status==hzuo)//头部正转
      {
        motor_contrl_send(0x01,0x01,20,0);    
      }
      if(lcd_status==hyou)//头部反转
      {
        motor_contrl_send(0x01,0x02,20,0);        
      }
      if(lcd_status==hzhong)//头部正中
      {
        motor_contrl_send( 0x01,0x03,20,180);
        delayms(1300);
        motor_contrl_send( 0x01,0x00,0,0);        
      }//*/

      //左手控制部分
      if(lcd_status==zbiaoding)//左手标定检测
      {
        motor_contrl_send( 0x06,0x05,0,0);
        delayms(10);
        motor_contrl_send( 0x06,0x05,0,0);
        delayms(10);         
        motor_contrl_send( 0x06,0x00,0,0);//退出标定状态
      }
      if(lcd_status==zzuo)//左手正转
      {
        motor_contrl_send( 0x06,0x01,7,0);    
      }
      if(lcd_status==zyou)//左手反转
      {
        motor_contrl_send( 0x06,0x02,7,0);        
      }
      if(lcd_status==zzhong)//左手正中
      {
        motor_contrl_send( 0x06,0x03,7,180);
        delayms(1800);
        motor_contrl_send( 0x06,0x00,0,0);        
      }

      //*右手控制部分
      if(lcd_status==ybiaoding)//左手标定检测
      {
        motor_contrl_send( 0x07,0x05,0,0);
        delayms(10);
        motor_contrl_send( 0x07,0x05,0,0);
        delayms(10);         
        motor_contrl_send( 0x07,0x00,0,0);//退出标定状态
      }
      if(lcd_status==yzuo)//左手正转
      {
        ////printf("yzuo\n");
        motor_contrl_send( 0x07,0x01,7,0);    
      }
      if(lcd_status==yyou)//左手反转
      {
        ////printf("yyou\n");
        motor_contrl_send( 0x07,0x02,7,0);        
      }
      if(lcd_status==yzhong)//左手正中
      {
        ////printf("yzhong\n");
        motor_contrl_send( 0x07,0x03,7,180);
        delayms(1800);
        motor_contrl_send( 0x07,0x00,0,0);        
      }

      //LED灯板控制部分
      if(lcd_status==led00)
      {
        //printf("led00\n");
        led_contrl_send( 0x03,0,0,100);
      }
      if(lcd_status==led01)
      {
        //printf("led01\n");
        led_contrl_send( 0x03,0,35,100);
      }
      if(lcd_status==led10)
      {
        //printf("led10\n");
        led_contrl_send( 0x03,35,0,100);
      }
      if(lcd_status==led11)
      {
        //printf("led11\n");
        led_contrl_send( 0x03,35,35,100);
      }

      //连续控制部分
      if(lcd_status==alianxu1)
      {
        //printf("alianxu1\n");
        while(1)
        {
          motor_contrl_send( 0x01,0x01,20,0);//头部正转
          delayms(30);
          motor_contrl_send( 0x06,0x01,7,0);//左手正转 
          delayms(30);
          motor_contrl_send( 0x07,0x01,7,0);//右手正转 
          delayms(30);
          if(lcd_status==alianxu0) break;//退出检测     
          delayms(2000);
          if(lcd_status==alianxu0) break;//退出检测     
          motor_contrl_send( 0x01,0x02,20,0);//头部反转       
          delayms(30);
          motor_contrl_send( 0x06,0x02,7,0);//左手反转
          delayms(30);
          motor_contrl_send( 0x07,0x02,7,0);//右手反转
          delayms(30);
          if(lcd_status==alianxu0) break;//退出检测     
          delayms(2000);

          if(lcd_status==alianxu0) break;//退出检测       
        }
        //printf("alianxu0\n");
        //退出回正
        motor_contrl_send( 0x01,0x03,20,180);//头回正
        delayms(30);
        motor_contrl_send( 0x06,0x03,7,180);//左手回正
        delayms(30);
        motor_contrl_send( 0x07,0x03,7,180);//右手回正
        delayms(30);
        delayms(1800);//等待回正完毕

        //退出后设置为自然停止
        motor_contrl_send( 0x01,0x00,0,0);//头停止 
        delayms(30);
        motor_contrl_send( 0x06,0x00,0,0);//左手停止 
        delayms(30);
        motor_contrl_send( 0x07,0x00,0,0);//右手停止
        delayms(30);      
      }//*/

     if(lcd_status==Return_button)//按下退出按键
     {
        //printf("return_button\n");
        Lcd_control( "page main");
        lcd_status = main_window;
        //退出回正
        motor_contrl_send( 0x01,0x03,20,180);//头回正
        delayms(30);
        motor_contrl_send( 0x06,0x03,7,180);//左手回正
        delayms(30);
        motor_contrl_send( 0x07,0x03,7,180);//右手回正
        delayms(30);
        delayms(1800);//等待回正完毕
        //退出后设置为自然停止
        motor_contrl_send( 0x01,0x00,0,0);//头停止 
        delayms(30);
        motor_contrl_send( 0x06,0x00,0,0);//左手停止 
        delayms(30);
        motor_contrl_send( 0x07,0x00,0,0);//右手停止
        delayms(30); 

        return -1;//如果受到返回信号
     }

     lcd_status=0;//状态清零
   return 0;//正常刷新数据
}

