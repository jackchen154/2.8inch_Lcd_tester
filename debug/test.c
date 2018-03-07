#include <pthread.h>
#include <stdio.h>
#include<unistd.h>     /*Unix 标准函数定义read(),write(),sleep()*/
#include<stdlib.h>     /*标准函数库定义exit()*/
#include<string.h> 
#include "uart.h"
#include "lcd_window.h"
#include "test.h"
#define uchar unsigned char
#define FALSE  -1
#define TRUE   0

//reg_nam为寄存器的具体名称 
//enum reg_ename {zuolicheng=3,youlicheng,chaokuandaiX,chaokuandaiY,chaosheng1=11,chaosheng2,chaosheng3,chaosheng4,chaosheng5,chaosheng6,hongwai1=21,hongwai2,dianliang1=25,dianliang2};
//reg_data为寄存器名称对应的实际地址 
uchar reg[]={0x78,0x79,0x7a,0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A};

//该为串口1(主控板)的接收全局变量
//uchar data_buf[100]={0},start_recive=0,start_fram=0,frame_len=0,data_len=0;
//real_data寄存器地址所存放的数据(一个寄存器地址的数据为2字节) ,请求响应寄存器后接收解析的数据将会保存到这里 
unsigned short real_data[50]={0};
int send_data(int fd,uchar rw,uchar device_n,uchar reg_n,uchar reg_mun);

//enum window { Sensor_window, Control_window, Headctr_window, Hardwar_window, Return_button};//窗口页面名称
//uchar lcd_status=0;//LCD_window的切换状态


//毫秒级别延时函数
void  delayms(unsigned int ms)
{
   while(ms--)
   usleep(1000);
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



/*
将数据进行发送
fd为为文件描述符
rw读写操作(0为读操作，1为写操作)
device_n为设备地址
reg_n为寄存器起始地址
reg_mun为要读/写的寄存器个数
*/
int send_data(int fd,uchar rw,uchar device_n,uchar reg_n,uchar reg_mun)
{
	unsigned int crc,i,len;
	uchar read_buf[12]={0xaa,0x55,0xcc};
	//uchar write_buf[12]={0xaa,0x55,0xcc};
	if(rw==0)
	{
	  read_buf[3]=device_n;
	  read_buf[4]=0x03;
	  read_buf[5]=0x00;
	  read_buf[6]=reg_n;
	  read_buf[7]=0x00;
	  read_buf[8]=reg_mun;
	  crc=get_crc(&read_buf[3],6);
	  read_buf[9]=crc&0xff;
	  read_buf[10]=(crc>>8)&0xff;
	  //printf("\n\nsend data is:");
	  /*for(i=0;i<11;i++)

	  printf("%x ",read_buf[i]);
 	  printf("  ");
	 */
	  len = UART_Send(fd,read_buf,11);
          if (len == 11 ) 
 	  {
             //printf("send sucess!\n\n");
             return len;
          }
          else   
          {               
            UART_clear_buf(fd);
            return FALSE;
          }
	}

   return TRUE;
}

//(fd0,05,05,20,180)
int motor_contrl_send(int fd, uchar device_n,uchar ctrl_mode,uchar speed,unsigned int weizhi)
{
      unsigned int crc,i,len;
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

	  crc=get_crc(&read_buf[3],21);
	  read_buf[24]=crc&0xff;
	  read_buf[25]=(crc>>8)&0xff;
	  //*printf("\n\nsend data is:");
	  for(i=0;i<26;i++)

	  printf("%x ",read_buf[i]);
 	  printf("  ");
	 //*/
	  len = UART_Send(fd,read_buf,26);
      if (len == 26 ) 
 	  {
             printf("send sucess!\n\n");
             return len;
      }
      else   
      {               
         UART_clear_buf(fd);
         return FALSE;
      }
	
}


int led_contrl_send(int fd, uchar device_n,uchar led1_cycle,uchar led2_cycle,uchar liangdu)
{
      unsigned int crc,len;
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


	  crc=get_crc(&read_buf[3],13);
	  read_buf[14]=crc&0xff;
	  read_buf[15]=(crc>>8)&0xff;
	  //printf("\n\nsend data is:");
	  /*for(i=0;i<11;i++)

	  printf("%x ",read_buf[i]);
 	  printf("  ");
	 */
	  len = UART_Send(fd,read_buf,16);
      if (len == 16 ) 
 	  {
             //printf("send sucess!\n\n");
             return len;
      }
      else   
      {               
         UART_clear_buf(fd);
         return FALSE;
      }
	
}

//*
void *lcdjieshou(void* fd2)
{
 //该为串口2lcd的接收全局变量
 uchar data_buf[20]={0},start_recive=0,start_fram=0,frame_len=0,data_len=0;
 int len;
 uchar rcv_buf;
 uchar i,j;
 unsigned short crc_result,ck_crc;
 unsigned short real_data1[1]; 
 while(1)
 { 

   len=UART_Recv(*((int *)fd2), &rcv_buf,1);
   if(len>0)
   {
     data_buf[data_len]=rcv_buf;    
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
           continue;
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
	           real_data1[i]=data_buf[6+j]<<8| data_buf[7+j];//有效数据合为2个字节的数据（一个寄存器的数据值为2个字节） 
	           j=j+2;
	         }	         
           }            
           start_recive=0;
           start_fram=0;
		   data_len=0;
		   frame_len=0;
		   j=0; i=0;

           //页面窗口部件
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
           //头部窗口部件
           if(real_data1[0]==0x0300)
           {
             //printf("hbiaoding\n");
             lcd_status=hbiaoding;
           }
           if(real_data1[0]==0x0301)
           {
             //printf("hzuo\n");
             lcd_status=hzuo;
           } 
           if(real_data1[0]==0x0302)
           {
             //printf("hzhong\n");
             lcd_status=hzhong;
           } 
           if(real_data1[0]==0x0303)
           {
             //printf("hyou\n");
             lcd_status=hyou;
           }

           //左手窗口部件
           if(real_data1[0]==0x031a)
           {
             printf("jieshouzbiaoding\n");
             lcd_status=zbiaoding;
           }
           if(real_data1[0]==0x0310)
           {
             printf("jieshouzzuo\n");
             lcd_status=zzuo;
           } 
           if(real_data1[0]==0x0312)
           {
             printf("jieshouzzhong\n");
             lcd_status=zzhong;
           } 
           if(real_data1[0]==0x031b)
           {
             printf("jieshouzyou\n");
             lcd_status=zyou;
           }

           //右手窗口部件
           if(real_data1[0]==0x0320)
           {
             //printf("ybiaoding\n");
             lcd_status=ybiaoding;
           }
           if(real_data1[0]==0x0321)
           {
             //printf("yzuo\n");
             lcd_status=yzuo;
           } 
           if(real_data1[0]==0x0322)
           {
             //printf("yzhong\n");
             lcd_status=yzhong;
           } 
           if(real_data1[0]==0x0323)
           {
             //printf("yyou\n");
             lcd_status=yyou;
           }

           //LED灯板控制部件 
           if(real_data1[0]==0x0330)
           {
             //printf("zui0\n");
             lcd_status=zui0;
           } 
           if(real_data1[0]==0x0331)
           {
             //printf("zui1\n");
             lcd_status=zui1;
           } 
           if(real_data1[0]==0x0332)
           {
             //printf("yan0\n");
             lcd_status=yan0;
           } 
           if(real_data1[0]==0x0333)
           {
             //printf("yan1\n");
             lcd_status=yan1;
           }

           //连续控制部件 
           if(real_data1[0]==0x0334)
           {
             //printf("lianxu0\n");
             lcd_status=alianxu0;
           } 
           if(real_data1[0]==0x0335)
           {
             //printf("lianxu1\n");
             lcd_status=alianxu1;
           }
           real_data1[0]=0x0000;           
           continue;
        }
      }        
      if(data_len==5)
      {
        start_fram=1;
        frame_len=data_buf[5]+2;
      }
      data_len++;
    }
    len=0;
   }
 }//while end
 return 0;   
}

void *zhukongjieshou(void *fd1)//串口接收的桢解析(线程1)
{
 //该为串口1(主控板)的接收全局变量
 uchar data_buf[100]={0},start_recive=0,start_fram=0,frame_len=0,data_len=0;
 int len;
 uchar rcv_buf;
 uchar i,j;
 unsigned short crc_result,ck_crc; 
 while(1)
 { 

   len=UART_Recv(*((int *)fd1), &rcv_buf,1);
   if(len>0)
   {
     data_buf[data_len]=rcv_buf;    
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
           continue;
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
           //printf("lala\n");
           continue;
        }
      }        
      if(data_len==5)
      {
        start_fram=1;
        frame_len=data_buf[5]+2;
      }
      data_len++;
    }
    len=0;
   }
 }//while end
 return 0;  
}

int main(int argc, char **argv)
{

    int fd1=0; //串口1    
    int fd2=0; //串口2
    pthread_t com1_id;
    pthread_t com2_id;

    
    fd1 = UART_Open(fd1,"/dev/ttyS0"); //打开串口1，返回文件描述符
    //fd1 = UART_Open(fd1,"/dev/ttyUSB0"); //打开串口1，返回文件描述符
    if(fd1<0)
    { 
      printf("open port1 fail!\n");
      exit(1);
    }

    if(UART_Init(fd1,0,38400,'N',8,1)<0)
    {
       printf("UART_Init1 fail!\n");
       exit(1); 
    }

    fd2 = UART_Open(fd2,"/dev/ttyS1"); //打开串口2，返回文件描述符
    if(fd2<0)
    { 
      printf("open port2 fail!\n");
      exit(1);
    }

    if(UART_Init(fd2,0,9600,'N',8,1)<0)
    {
       printf("UART_Init2 fail!\n");
       exit(1); 
    }
    
    if(pthread_create(&com1_id,NULL,zhukongjieshou,(void *)&fd1)>0)//启动主控板接收线程
    {
        printf("the pthread zhukongjieshou faill!\n");
        exit(1);
    }

    
    if(pthread_create(&com2_id,NULL,lcdjieshou,(void *)&fd2)>0)
    {
        printf("the pthread lcdjieshou faill!\n");
        exit(1);
    }

    Lcd_control(fd2,"page main");     
    while(1)
    { 

          if(lcd_status==Sensor_window)
          {   
              Lcd_control(fd2,"page ultrasonic");     
              while (1) //循环读取数据
              { 
                send_data(fd1,0,0x81,0x80,27); //主控板数据请求         
                if(ultrasonic_window(fd2,real_data)<0) break;
                sleep(1);
              }
           }

           if(lcd_status==versions_window)
           {   
              Lcd_control(fd2,"page version");     
              while (1) //循环读取数据
              { 
                //send_data(fd1,0,0x81,0x80,27); //主控板数据请求         
                if(version_window(fd1,fd2,real_data)<0) break;
                delayms(100);
              }
           } 
           if(lcd_status==Headctr_window)
           {   
              Lcd_control(fd2,"page head contrl"); 
              motor_contrl_send(fd1,0x01,0x00,0,0);//初始化舵机
              delayms(5);
              motor_contrl_send(fd1,0x06,0x00,0,0);//初始化舵机
              delayms(5);  
              motor_contrl_send(fd1,0x07,0x00,0,0);//初始化舵机     
              while (1) //循环读取数据
              { 
                //send_data(fd1,0,0x81,0x80,27); //主控板数据请求         
                if(motor_ctrl_window(fd1,fd2,real_data)<0) break;
                delayms(5);
              }
           } 
        delayms(100);
        //printf("main window\n"); 
      }//end while(1)

     UART_Close(fd1);
}


