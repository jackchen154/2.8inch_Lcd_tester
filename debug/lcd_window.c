#include <stdio.h>
#include "uart.h"
#include "lcd_window.h"
#define uchar unsigned char
#define FALSE  -1
#define TRUE   0
enum reg_enum {zuolicheng=3,youlicheng,chaokuandaiX,chaokuandaiY,chaosheng1=11,chaosheng2,chaosheng3,chaosheng4,chaosheng5,chaosheng6,hongwai1=21,hongwai2,dianliang1=25,dianliang2};

int Lcd_control(int fd, char *cmd)//模式设置
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
   
  len = UART_Send(fd,(uchar *)cmd,len);//发送LCD控制指令
  if (len < 0 ) 
  {
    printf("lcd_control发送失败\n");        
    UART_clear_buf(fd);//清空串口缓存
    return FALSE;
  }
  
  UART_Send(fd,end_frame,3);//发送LCD帧尾

  /*
  if(UART_Recv(fd,receive_buf,4)<0)
  {
     printf("接收超时！请检查模块是否连接正常。\n");
     return FALSE;
  }   
    
  if(receive_buf[0]==0x1a&&receive_buf[1]==0xff&&receive_buf[2]==0xff)
  { 
      printf("变量名称无效\n"); 
      return FALSE;
  }
  if(receive_buf[0]==0x00&&receive_buf[1]==0xff&&receive_buf[2]==0xff)
  { 
     printf("无效指令\n"); 
     return FALSE;
  }
  //*/
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
  unsigned char buf[6]; // 长整数最大值4294967295，转ASCII码后占用10+1=11字节
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

/*int char_compare(uchar *ch1,char *ch2)//字符串对比，相同返回0,不同返回-1
{
  uchar count1,count2;
  uchar *cha = ch1;
  char *chb = ch2;
  while(*cha!='\0')
  {
    cha++;
    count1++;
  }
  while(*chb!='\0')
  {
    chb++;
    count2++;
  }
  if(count1 != count2)
  return FALSE;

  while(*ch2!='\0')
  {
    if(*ch1==*ch2)
    {
      ch1++;
      ch2++;
    }
    else
    return FALSE;
  }
  return TRUE;
}*/

int Lcd_set_val(int fd, char *cmd, int val)//模式设置
{
  //uchar receive_buf[4];
  uchar val_buf[5];//int转char的缓存 
  uchar val_len;//转换后的数字有多少位数
  uchar *cmd_buf = (uchar *)cmd;
  uchar end_frame[]={0xff,0xff,0xff};//帧尾
  uchar cmd_len=0;//命令字的长度

  val_len = int2str(val,val_buf);//整型转字符型

  while(*cmd_buf!='\0')
  {
    cmd_buf++;
    cmd_len++;
  }

  cmd_len = UART_Send(fd,(uchar *)cmd,cmd_len);//发送LCD控制指令
  if (cmd_len < 0 ) 
  {
     printf("Lcd_set_val发送失败\n");              
     UART_clear_buf(fd);
     return FALSE;
  }

  UART_Send(fd,val_buf,val_len);//发送数字 

  UART_Send(fd,end_frame,3);//帧尾部
 
  /*
  if(UART_Recv(fd,receive_buf,4)<0)
  {
       printf("接收超时！请检查模块是否连接正常。\n");
       return FALSE;
    }   
    
    if(receive_buf[0]==0x1a&&receive_buf[1]==0xff&&receive_buf[2]==0xff)
    { 
        printf("变量名称无效\n"); 
        return FALSE;
    }
    if(receive_buf[0]==0x00&&receive_buf[1]==0xff&&receive_buf[2]==0xff)
    { 
        printf("无效指令\n"); 
        return FALSE;
    }//*/
    return TRUE;    

}

void ultrasonic_window(int fd,unsigned short *real_data)// 传感器窗口数据刷新
{        
                              		
   Lcd_set_val(fd,"d0.val=",(*(real_data+chaosheng1) >> 8)*10);
   Lcd_set_val(fd,"d2.val=",(*(real_data+chaosheng1) & 0x00ff)*10);
   Lcd_set_val(fd,"d4.val=",(*(real_data+chaosheng2) >>8)*10);
   Lcd_set_val(fd,"d6.val=",(*(real_data+chaosheng2) & 0x00ff)*10);
   Lcd_set_val(fd,"d8.val=",(*(real_data+chaosheng3) >> 8)*10);
   Lcd_set_val(fd,"da.val=",(*(real_data+chaosheng3) & 0x00ff)*10);
   Lcd_set_val(fd,"dc.val=",(*(real_data+chaosheng4) >> 8)*10);
   Lcd_set_val(fd,"de.val=",(*(real_data+chaosheng4) & 0x00ff)*10);
   Lcd_set_val(fd,"e0.val=",(*(real_data+chaosheng5) >> 8)*10);
   Lcd_set_val(fd,"e2.val=",(*(real_data+chaosheng5) & 0x00ff)*10);
   Lcd_set_val(fd,"e4.val=",(*(real_data+chaosheng6) >> 8)*10);
   Lcd_set_val(fd,"e8.val=",(*(real_data+chaosheng6) & 0x00ff)*10);
   Lcd_set_val(fd,"hongwaizuo.val=",(*(real_data+hongwai2) >> 8)*2);
   Lcd_set_val(fd,"hongwaiyou.val=",(*(real_data+hongwai2) & 0x00ff)*2);
   //printf("the touch is:%d",Lcd_touch_read(fd2));
}


