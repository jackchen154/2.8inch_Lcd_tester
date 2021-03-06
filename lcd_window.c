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
  delayms(10);
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


//tou_led.txt="V12.00.02"
//该为自带清buf的步骤
int Lcd_set_txt(char *cmd,unsigned short *real_data)//模式设置
{

  uchar *cmd_buf = (uchar *)cmd;
  uchar clear_data=50;
  uchar end_frame[]={0xff,0xff,0xff};//帧尾
  uchar disbuf[11]={0};
  uchar cmd_len=0;

  disbuf[0]='"';
  disbuf[1]='V';
	if((*(real_data)>>8)>9)//如果主版本号大于9
	{
		disbuf[2]=(*(real_data)>>8)/10+48;
		disbuf[3]=(*(real_data)>>8)%10+48;
	}
	else
	{
	  disbuf[2]= '0';
		disbuf[3]=(*(real_data)>>8)+48;
	}
  disbuf[4]='.';
	
	if((*(real_data)&0x00ff)>9)//如果主版本号大于9
	{
		disbuf[5]=(*(real_data)&0x00ff)/10+48;
		disbuf[6]=(*(real_data)&0x00ff)%10+48;
	}
	else
	{
	  disbuf[5]= '0';
    disbuf[6]=(*(real_data)&0x00ff)+48;
	}

  disbuf[7]='.';

	if((*(real_data+1)>>8)>9)//如果主版本号大于9
	{
		disbuf[8]=(*(real_data+1)>>8)/10+48;
		disbuf[9]=(*(real_data+1)>>8)%10+48;
	}
	else
	{
	  disbuf[8]= '0';
    disbuf[9]=(*(real_data+1)>>8)+48;
	}	
  disbuf[10]='"';

  while(*cmd_buf!='\0')
  {
    cmd_buf++;
    cmd_len++;
  }

  UART2_Send((uchar *)cmd,cmd_len);//发送LCD控制指令
  
  if(*(real_data)==0 && *(real_data+1)==0)
  UART2_Send((uchar *)"\"no device\"",11);
  else
  UART2_Send(disbuf,11);//发送值 
  
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
	 
	 //读取版本号
	 send_data(0x81,0xCA,2); 
	 delayms(30);	 	 
	 Lcd_set_txt("val1.txt=",real_data);//0号板
	 send_data(0x81,0xCC,2); 
	 delayms(30);	 
	 Lcd_set_txt("val2.txt=",real_data);//1号板
	 send_data(0x81,0xCE,2); 
	 delayms(30);	 
	 Lcd_set_txt("val3.txt=",real_data);//2号板
	 send_data(0x81,0xD0,2); 
	 delayms(30);	 
	 Lcd_set_txt("val4.txt=",real_data);//3号板
	 
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
int ultrasonic_window1(unsigned short *real_data)// 传感器窗口数据刷新
{        
   uchar clear_data=50; 

	 send_data(0x81,0x8b,16); 
	 delayms(30);
   Lcd_set_val("d0.val=",(*(real_data) >> 8)*10);
   Lcd_set_val("d1.val=",(*(real_data) & 0x00ff)*10);
   Lcd_set_val("d2.val=",(*(real_data+1) >>8)*10);
   Lcd_set_val("d3.val=",(*(real_data+1) & 0x00ff)*10);
   Lcd_set_val("d4.val=",(*(real_data+2) >> 8)*10);
   Lcd_set_val("d5.val=",(*(real_data+2) & 0x00ff)*10);
   Lcd_set_val("d6.val=",(*(real_data+3) >> 8)*10);
   Lcd_set_val("d7.val=",(*(real_data+3) & 0x00ff)*10);
   Lcd_set_val("d8.val=",(*(real_data+4) >> 8)*10);
   Lcd_set_val("d9.val=",(*(real_data+4) & 0x00ff)*10);
   Lcd_set_val("d10.val=",(*(real_data+5) >> 8)*10);
   //Lcd_set_val("e8.val=",(*(real_data+5) & 0x00ff)*10);
   Lcd_set_val("hongwaizuo.val=",(*(real_data+6) & 0x00ff)*2);
   Lcd_set_val("hongwaiyou.val=",(*(real_data+7) & 0x00ff)*2);
	 
	 /*/读取版本号
	 send_data(0x81,0xCA,2); 
	 delayms(30);	 	 
	 Lcd_set_txt("val1.txt=",real_data);//0号板
	 send_data(0x81,0xCC,2); 
	 delayms(30);	 
	 Lcd_set_txt("val2.txt=",real_data);//1号板
	 send_data(0x81,0xCE,2); 
	 delayms(30);	 
	 Lcd_set_txt("val3.txt=",real_data);//2号板
	 send_data(0x81,0xD0,2); 
	 delayms(30);	 
	 Lcd_set_txt("val4.txt=",real_data);//3号板
	 */
	 
	 //超声波1
	 send_data(0x81,0xcc,3);
   delayms(30);
   Lcd_set_txt("chaosheng1.txt=",real_data);
	  //超声波2
	 send_data(0x81,0xcf,3);
   delayms(30);
   Lcd_set_txt("chaosheng2.txt=",real_data);
			
	 while(clear_data--)
   {
    *real_data=0x0000;
    real_data++;
   }
	
   if(lcd_status==Return_button1)
   {
        Lcd_control("page main1");
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
      //单测右轮
      send_data(0x02,0x78,3);
      delayms(30);
      Lcd_set_txt("danyoulun.txt=",real_data);
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
int version_window1(unsigned short *real_data)// 传感器窗口数据刷新
{        
       /*TH1 = 0xf7;	
	     TL1 = TH1;	//38400波特率 
			 delayms(30);

      //单测左轮
      send_data(0x01,0x78,3);
      delayms(30);
      Lcd_set_txt("danzuolun.txt=",real_data);
      //单测右轮
      send_data(0x02,0x78,3);
      delayms(30);
      Lcd_set_txt("danyoulun.txt=",real_data);*/
	
			//主控板
			send_data(0x81,0x78,3);
      delayms(30);
      Lcd_set_txt("zhukong.txt=",real_data);
			//超宽带
			send_data(0x81,0xc0,3);
      delayms(30);
      Lcd_set_txt("chaokuandai.txt=",real_data);			
			//左轮
			send_data(0x81,0xc6,3);
      delayms(30);
      Lcd_set_txt("zuolun.txt=",real_data);	   
			//右轮
			send_data(0x81,0xc9,3);
      delayms(30);
      Lcd_set_txt("youlun.txt=",real_data);
			//电源管理
			send_data(0x81,0xc3,3);
      delayms(30);
      Lcd_set_txt("dianyuanguanli.txt=",real_data);		
			//超声波1
			send_data(0x81,0xcc,3);
      delayms(30);
      Lcd_set_txt("chaosheng1.txt=",real_data);
			//超声波2
			send_data(0x81,0xcf,3);
      delayms(30);
      Lcd_set_txt("chaosheng2.txt=",real_data);
			
      //舵机板
      send_data(0x82,0x70,3);
      delayms(30);
      Lcd_set_txt("tou_duoji.txt=",real_data);
			delayms(30);
			
			//驱动器选择
      if(lcd_status==feng_de_kong)//风得控
      {
        main_board_single_write(0x0f,0x047f);//主控板单写   
        delayms(30);	
        main_board_single_write(0x09,0x5a5a);//写入flash  
        delayms(200);	
        main_board_single_write(0x5a,0x5555);//重启主控板
        delayms(30);
				qu_dong_qi=1;
        lcd_status=0;				
      }	
      if(lcd_status==yi_ban_qudong)//一般3代驱动
      {
        main_board_single_write(0x0f,0x007f);//主控板单写   
        delayms(30);	
        main_board_single_write(0x09,0x5a5a);//写入flash  
        delayms(200);	
        main_board_single_write(0x5a,0x5555);//重启主控板
        delayms(30);
				qu_dong_qi=0;
        lcd_status=0;				
      }				
     if(lcd_status==Return_button1)
     {
        Lcd_control("page main1");
        lcd_status = main_window;
        return -1;//如果受到返回信号
     }
   
   return 0;//正常刷新数据
}

int motor_ctrl_window(unsigned short *real_data)//舵机控制窗口
{        
         
      uchar turn_speed = 10;
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
        delayms(30);
        motor_contrl_send(0x01,0x05,0,0);
        delayms(30);         
        motor_contrl_send(0x01,0x00,0,0);
      }
      if(lcd_status==hzuo)//头部正转
      {
        motor_contrl_send(0x01,0x01,turn_speed,0);    
      }
      if(lcd_status==hyou)//头部反转
      {
        motor_contrl_send(0x01,0x02,turn_speed,0);        
      }
      if(lcd_status==hzhong)//头部正中
      {
        motor_contrl_send( 0x01,0x03,turn_speed,180);
        delayms(2000);
        motor_contrl_send( 0x01,0x00,0,0);        
      }//*/

      //左手控制部分
      if(lcd_status==zbiaoding)//左手标定检测
      {
        motor_contrl_send( 0x06,0x05,0,0);
        delayms(30);
        motor_contrl_send( 0x06,0x05,0,0);
        delayms(30);         
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
        delayms(2000);
        motor_contrl_send( 0x06,0x00,0,0);        
      }

      //*右手控制部分
      if(lcd_status==ybiaoding)//左手标定检测
      {
        motor_contrl_send( 0x07,0x05,0,0);
        delayms(30);
        motor_contrl_send( 0x07,0x05,0,0);
        delayms(30);         
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
        delayms(2000);
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
          motor_contrl_send( 0x01,0x01,9,0);//头部正转
          delayms(30);
          motor_contrl_send( 0x06,0x01,7,0);//左手正转 
          delayms(30);
          motor_contrl_send( 0x07,0x01,7,0);//右手正转 
          delayms(30);
          if(lcd_status==alianxu0) break;//退出检测
					if(lcd_status==Return_button) break;
          delayms(3500);
					if(lcd_status==Return_button) break;
          if(lcd_status==alianxu0) break;//退出检测     
          motor_contrl_send( 0x01,0x02,9,0);//头部反转       
          delayms(30);
          motor_contrl_send( 0x06,0x02,7,0);//左手反转
          delayms(30);
					if(lcd_status==Return_button) break;
          motor_contrl_send( 0x07,0x02,7,0);//右手反转
          delayms(30);
          if(lcd_status==alianxu0) break;//退出检测     
          delayms(3500);
					if(lcd_status==Return_button) break;
          if(lcd_status==alianxu0) break;//退出检测       
        }
        //printf("alianxu0\n");
        //退出回正
        motor_contrl_send( 0x01,0x03,9,180);//头回正
        delayms(30);
        motor_contrl_send( 0x06,0x03,7,180);//左手回正
        delayms(30);
        motor_contrl_send( 0x07,0x03,7,180);//右手回正
        delayms(30);
        delayms(2000);//等待回正完毕

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
        motor_contrl_send( 0x01,0x03,9,180);//头回正
        delayms(30);
        motor_contrl_send( 0x06,0x03,7,180);//左手回正
        delayms(30);
        motor_contrl_send( 0x07,0x03,7,180);//右手回正
        delayms(30);
        delayms(2000);//等待回正完毕
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


//*

int motor_ctrl_window1(unsigned short *real_data)//舵机控制窗口
{        
         
      uchar turn_speed = 15;//15最大值
	    int head_angle=0,left_angle=0,right_angle=0;
	    //*用于心跳检测间隔
	    send_data(0x82,0x70,3);
      delayms(200); //*/
      //头部控制部分  
      if(lcd_status==hbiaoding)//标定检测
      {
        motor_contrl_send1(0x05,0x01,0,0);//头标定
        delayms(30);
        motor_contrl_send1(0x16,0x05,0,0);//左手标定
        delayms(30);         
        motor_contrl_send1(0x22,0x05,0,0);//右手标定
      }
      if(lcd_status==hzuo)//头部正转
      {
        motor_contrl_send1(0x05,0x04,turn_speed,0);    
      }
      if(lcd_status==hyou)//头部反转
      {
        motor_contrl_send1(0x05,0x04,turn_speed,180);        
      }
      if(lcd_status==hzhong)//头部正中
      {
        motor_contrl_send1( 0x05,0x04,turn_speed,90);      
      }

      //左手控制部分
      if(lcd_status==zzuo)//左手正转
      {
        motor_contrl_send1( 0x16,0x03,turn_speed,360);    
      }
      if(lcd_status==zyou)//左手反转
      {
        motor_contrl_send1( 0x16,0x03,turn_speed,120);        
      }
      if(lcd_status==zzhong)//左手正中
      {
        motor_contrl_send1( 0x16,0x03,turn_speed,180);     
      }

      //*右手控制部分
      if(lcd_status==yzuo)//正转
      {
        motor_contrl_send1( 0x22,0x03,turn_speed,360);    
      }
      if(lcd_status==yyou)//反转
      {
        motor_contrl_send1( 0x22,0x03,turn_speed,120);        
      }
      if(lcd_status==yzhong)//正中
      {
        motor_contrl_send1( 0x22,0x03,turn_speed,180);     
      }

      //LED灯板控制部分
      if(lcd_status==led00)//关闭
      {
        led_contrl_send1( 0x00,0,0,0);
      }
      if(lcd_status==led01)//红
      {
        led_contrl_send1( 0x01,255,0,0);
      }
      if(lcd_status==led10)//绿
      {       
        led_contrl_send1( 0x01,0,255,0);
      }
      if(lcd_status==led11)//蓝
      {
        led_contrl_send1( 0x01,0,0,255);
      }

      //连续控制部分
      if(lcd_status==alianxu1)
      {
        motor_contrl_send1( 0x05,0x04,turn_speed,0);//头部正转
        delayms(30);
        motor_contrl_send1( 0x16,0x03,turn_speed,360);//左手正转 
        delayms(30);
        motor_contrl_send1( 0x22,0x03,turn_speed,360);//右手正转 
        while(1)
        {				
          send_data(0x82,0x74,11);
					delayms(30);
          head_angle=(int)(*(real_data));
					left_angle=(int)(*(real_data+5));
					right_angle=(int)(*(real_data+10));
					
					if(head_angle>=0&&head_angle<=2)
					{
					   motor_contrl_send1( 0x05,0x04,turn_speed,180);//头部反转
						 delayms(30);
					}
					if(head_angle>=178&&head_angle<=180)
					{
             motor_contrl_send1( 0x05,0x04,turn_speed,0);//头部正转 
             delayms(30);						
					}

					//左手
					if(left_angle>=358&&left_angle<=360||left_angle>=0&&left_angle<=2)
					{
          motor_contrl_send1( 0x16,0x03,turn_speed,120);//左手反转 
						 delayms(30);
					}
					if(left_angle>=117&&left_angle<=122)
					{
             motor_contrl_send1( 0x16,0x03,turn_speed,360);//左手正转 
             delayms(30);						
					}	
					//右手
					if(right_angle>=358&&right_angle<=360||right_angle>=0&&right_angle<=2)
					{
           motor_contrl_send1( 0x22,0x03,turn_speed,120);//右手反转 
						 delayms(30);
					}
					if(right_angle>=117&&right_angle<=122)
					{
             motor_contrl_send1( 0x22,0x03,turn_speed,360);//右手正转 
             delayms(30);						
					}						

					if(lcd_status==Return_button1) break;
          if(lcd_status==alianxu0) break;//退出检测     
          delayms(200);
        }

        //退出回正

        motor_contrl_send1( 0x16,0x03,turn_speed,180);//左手回正
        delayms(30);
        motor_contrl_send1( 0x22,0x03,turn_speed,180);//右手回正
        delayms(30);
        motor_contrl_send1( 0x05,0x04,turn_speed,90);//头回正
        delayms(30);				

   
      }

     if(lcd_status==Return_button1)//按下退出按键
     {
        //printf("return_button\n");
        Lcd_control( "page main1");
        lcd_status = main_window;
        //退出回正
        motor_contrl_send1( 0x05,0x04,turn_speed,90);//头回正
        delayms(30);
        motor_contrl_send1( 0x16,0x03,turn_speed,180);//左手回正
        delayms(30);
        motor_contrl_send1( 0x22,0x03,turn_speed,180);//右手回正


        return -1;//如果受到返回信号
     }

    lcd_status=0;//状态清零
   return 0;//正常刷新数据
}


int Duojixianzhi_window(unsigned short *real_data)//舵机限制窗口
{                
      //头部限制查询
    	send_data(0x01,0x80,1);//读限定电流值
      delayms(30);
      Lcd_set_val("htouxianliu.val=", *(real_data));
      delayms(30);
	    *(real_data)=0;
	
	    send_data(0x01,0x0B,2);//读限位值
	    delayms(30);
      Lcd_set_val("htoushangxw.val=", (int)(*(real_data)));
	    delayms(30);
      Lcd_set_val("htouxiaxw.val=", (int)(*(real_data+1)));
      *(real_data)=0;
      *(real_data+1)=0;

      //俯仰限制查询
    	send_data(0x02,0x80,1);//读限定电流值
      delayms(30);
      Lcd_set_val("fyxianliu.val=", *(real_data));
      delayms(30);
	    *(real_data)=0;
	
	    send_data(0x02,0x0B,2);//读限位值
	    delayms(30);
      Lcd_set_val("fyshangxianw.val=", (int)(*(real_data)));
	    delayms(30);
      Lcd_set_val("fyxiaxianw.val=", (int)(*(real_data+1)));
      *(real_data)=0;
      *(real_data+1)=0;

      //左手限制查询
    	send_data(0x06,0x80,1);//读限定电流值
      delayms(30);
      Lcd_set_val("zuoxianliu.val=", *(real_data));
      delayms(30);
	    *(real_data)=0;
	
	    send_data(0x06,0x0B,2);//读限位值
	    delayms(30);
      Lcd_set_val("zuoshangxw.val=", (int)(*(real_data)));
	    delayms(30);
      Lcd_set_val("zuoxiaxw.val=", (int)(*(real_data+1)));
      *(real_data)=0;
      *(real_data+1)=0;
			
      //右手限制查询
    	send_data(0x07,0x80,1);//读限定电流值
      delayms(30);
      Lcd_set_val("yshouxianliu.val=", *(real_data));
      delayms(30);
	    *(real_data)=0;
	
	    send_data(0x07,0x0B,2);//读限位值
	    delayms(30);
      Lcd_set_val("yshoushangxw.val=", (int)(*(real_data)));
	    delayms(30);
      Lcd_set_val("yshaouxiaxw.val=", (int)(*(real_data+1)));
      *(real_data)=0;
      *(real_data+1)=0;			
			
      //*一键限制配置部分
      if(lcd_status==set_limit_value)
      {
        //头部舵机 
				limit_current_contrl_send(0x01,10);//头0.6A的限制电流
				delayms(30);
        limit_value_contrl_send(0x01 ,90,90);//头90度限位值发送
        delayms(30);
        //俯仰舵机 				
        limit_current_contrl_send(0x02,10);//俯仰0.6A的限制电流
				delayms(30);
        limit_value_contrl_send(0x02 ,9,9);//平安俯仰9度限位值发送
        delayms(30);
        //左手舵机 
				limit_current_contrl_send(0x06,10);//头1A的限制电流
				delayms(30);
        limit_value_contrl_send(0x06 ,90,90);//头90度限位值发送
        delayms(30);
        //右手舵机 				
        limit_current_contrl_send(0x07,10);//俯仰1A的限制电流
				delayms(30);
        limit_value_contrl_send(0x07 ,90,90);//俯仰90度限位值发送
        delayms(30);					
      }

     if(lcd_status==Return_button)//按下退出按键
     {
        //printf("return_button\n");
        Lcd_control( "page main");
        lcd_status = main_window;
        return -1;//如果受到返回信号
     }

     lcd_status=0;//状态清零
   return 0;//正常刷新数据
}

int biaoqing_window(void)//表情窗口
{                
			switch(lcd_status)
			{
			  case zhengchang: biao_qing_contrl_send(0);break;
				case aixin: biao_qing_contrl_send(145);break;
				case chongdian: biao_qing_contrl_send(257);break;
				case daohang: biao_qing_contrl_send(277);break;
				default : break;
			}

     if(lcd_status==Return_button1)//按下退出按键
     {
        //printf("return_button\n");
        Lcd_control( "page main1");
        lcd_status = main_window;
        return -1;//如果受到返回信号
     }

     lcd_status=0;//状态清零
   return 0;//正常刷新数据
}


//void main_board_contrl_send(uchar run_mode,uchar left_speed,uchar right_speed);//主控板多写
int robot_ctrl_window(unsigned short *real_data)//整机控制部分
{        
      *real_data = 0xf1f0;  
			 switch(lcd_status)
       {
          case auto_charge_on : main_board_contrl_send(0x80,0,0);  break;
					case auto_charge_off : main_board_contrl_send(0x00,0,0);  break;
					case go_up : main_board_contrl_send(0x01,40,40);  break;
					case go_down : main_board_contrl_send(0x02,40,40); break;
					case go_left : main_board_contrl_send(0x04,0,25); break;
					case go_right : main_board_contrl_send(0x03,25,0); break;
					case go_stop : main_board_contrl_send(0x05,0,0); //被动刹车
												 delayms(30);
												 main_board_contrl_send(0x00,0,0); //空挡无阻力
						             break;
           default :break;
        }
		
     if(lcd_status==Return_button)//按下退出按键
     {
        //printf("return_button\n");
        Lcd_control( "page main");
        lcd_status = main_window;
			  main_board_contrl_send(0x05,0,0); //被动刹车
			  delayms(30);
			  main_board_contrl_send(0x00,0,0); //空挡无阻力
        return -1;//如果受到返回信号
     }
		 delayms(10);
     send_data(0x81,0x88,1); //发送查询定位状态指令
	   delayms(30);
     Lcd_set_val("jiaodu.val=",(int)(*(real_data)*0.01));//向LCD模组发送定位值
		 delayms(10);
		 
		 
     send_data(0x81,0x97,2); //查询IR接收状态
	   delayms(30);
		 if((*(real_data)>>8)!=0xf1)
		 {
       Lcd_set_val("ir4.val=",*(real_data)>>8);
		   delayms(10);
		   Lcd_set_val("ir3.val=",*(real_data)& 0x00ff);
		   delayms(10);
		   Lcd_set_val("ir2.val=",*(real_data+1)>>8);
		   delayms(10);
		   Lcd_set_val("ir1.val=",*(real_data+1)& 0x00ff);
		   delayms(10);
		 }
		 //delayms(100);
		 
     //lcd_status=0;//状态清零
   return 0;//正常刷新数据
}
//*
int robot_ctrl_window1(unsigned short *real_data)//整机控制部分
{        
 	     unsigned short run_speed = 250;//最大值700
       unsigned short tun_speed =100;  
	    *real_data = 0xf1f0;

			 switch(lcd_status)
       {
          case auto_charge_on : main_board_contrl_send1(0x80,0,0);  break;
					case auto_charge_off : main_board_contrl_send1(0x00,0,0);  break;
					case go_up : main_board_contrl_send1(0x01,run_speed,run_speed);  break;
					case go_down : main_board_contrl_send1(0x02,run_speed,run_speed); break;
					case go_left : main_board_contrl_send1(0x04,0,tun_speed); break;
					case go_right : main_board_contrl_send1(0x03,tun_speed,0); break;
					case go_stop : main_board_contrl_send1(0x05,0,0); //被动刹车
												 delayms(30);
												 main_board_contrl_send1(0x00,0,0); //空挡无阻力
						             break;
           default :break;
        }
		
     if(lcd_status==Return_button1)//按下退出按键
     {
        //printf("return_button\n");
        Lcd_control( "page main1");
        lcd_status = main_window;
			  main_board_contrl_send1(0x05,0,0); //被动刹车
			  delayms(30);
			  main_board_contrl_send1(0x00,0,0); //空挡无阻力
			  delayms(30);
        main_board_single_write(0x5a,0x5555);//重启主控板
        delayms(30);				 
        return -1;//如果受到返回信号
     }
		 delayms(10);
     send_data(0x81,0x88,1); //发送查询定位状态指令
	   delayms(30);
     Lcd_set_val("jiaodu.val=",(int)(*(real_data)*0.01));//向LCD模组发送定位值
		 delayms(10);
		 
		 
     send_data(0x81,0x99,2); //查询IR接收状态
	   delayms(30);
		 if((*(real_data)>>8)!=0xf1)
		 {
       Lcd_set_val("ir4.val=",*(real_data)>>8);
		   delayms(10);
		   Lcd_set_val("ir3.val=",*(real_data)& 0x00ff);
		   delayms(10);
		   Lcd_set_val("ir2.val=",*(real_data+1)>>8);
		   delayms(10);
		   Lcd_set_val("ir1.val=",*(real_data+1)& 0x00ff);
		   delayms(10);
		 }
		 //delayms(100);
		 
     //lcd_status=0;//状态清零
   return 0;//正常刷新数据
}//*/
