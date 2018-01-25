#include <pthread.h>
#include<stdio.h>      /*标准输入输出定义*/
#include<stdlib.h>     /*标准函数库定义exit()*/
#include<unistd.h>     /*Unix 标准函数定义read(),write()*/
//#include<sys/types.h> 
//#include<sys/stat.h>   
#include<fcntl.h>      /*文件控制定义open(),fcntl()*/
#include<termios.h>    /*PPSIX 终端控制定义*/
//#include<errno.h>      /*错误号定义*/
#include<string.h>
#define uchar unsigned char 
#define FALSE  -1
#define TRUE   0
#define Time_out 1 //串口超时时间设置
    //reg_nam为寄存器的具体名称 
    enum reg_enum {zuolicheng=3,youlicheng,chaokuandaiX,chaokuandaiY,chaosheng1=11,chaosheng2,chaosheng3,chaosheng4,chaosheng5,chaosheng6,hongwai1=21,hongwai2,dianliang1=25,dianliang2};
    //reg_data为寄存器名称对应的实际地址 
    uchar reg[]={0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A};

//该为串口1(主控板)的接收全局变量
uchar data_buf[100]={0},start_recive=0,start_fram=0,frame_len=0,data_len=0;
uchar reboot_cnt=0;
//real_data寄存器地址所存放的数据(一个寄存器地址的数据为2字节) ,请求响应寄存器后接收解析的数据将会保存到这里 
unsigned short real_data[50]={0};
int send_data(int fd,uchar rw,uchar device_n,uchar reg_n,uchar reg_mun);

enum window { Sensor_button, Control_button, Headctr_button, Hardwar_button, Return_button};//窗口页面名称

/*******************************************************************
* 名称：UART0_Open
* 功能：打开串口并返回串口设备文件描述
* 入口参数：fd:文件描述符
          port :串口号(ttyS0,ttyS1,ttyS2)
* 出口参数：正确返回为1，错误返回为0
*******************************************************************/
int UART_Open(int fd,const char* port)
{
  fd = open( port, O_RDWR|O_NDELAY|O_NOCTTY);//
  if (fd < 0)
  {
    printf("Can not open:%s\n",port);
    perror("Error");
    return(FALSE);
  }
  printf("%s:open success!\n",port);

  //恢复串口为阻塞状态                               
  if(fcntl(fd, F_SETFL, 0) < 0)
  {
     printf("set block failed!\n");
     return(FALSE);
  }     
            
  return fd;
}

/*******************************************************************
* 名称：UART0_Close
* 功能：关闭串口并返回串口设备文件描述
* 入口参数：fd:文件描述符     
* 出口参数：void
*******************************************************************/
void UART_Close(int fd)
{
    close(fd);
}
 


/*******************************************************************
* 名称：UART0_Init
* 功能：设置串口数据位，停止位和效验位
* 入口参数：fd:串口文件描述符
*         speed:串口速度
*         flow_ctrl:数据流控制
*         databits:数据位(取值为5,6,7，8)
*         stopbits:停止位(值为1或者2)
*         parity:效验类型(取值为N=无校验位,E=偶校验,O=奇校验,S=空格)
          例子：UART0_Init(fd,0,115200,'N',8,1);
*出口参数：正确返回为1，错误返回为0
*******************************************************************/
int UART_Init(int fd,int flow_ctrl,int speed,int parity,int databits,int stopbits)
{
  unsigned int   i;
  uchar speed_sta;
  int   speed_arr[] = { B115200,B57600,B38400,B19200, B9600, B4800, B2400, B1200, B300};
  int   name_arr[] =  {  115200, 57600, 38400, 19200,  9600,  4800,  2400,  1200,  300};
         
  struct termios options; 
   /*tcgetattr(fd,&options)得到与fd指向对象的相关参数，并将它们保存于options,该函数还可以测试配置是否正确，该串口是否可用等。
     若调用成功，函数返回值为0，若调用失败，函数返回值为1.
   */
   if(tcgetattr(fd,&options)!=0)
   {
      perror("SetupSerial 1");    
      return(FALSE); 
   }
   printf("UART_init:");
   //设置串口输入波特率和输出波特率
   for ( i= 0;  i < sizeof(speed_arr) / sizeof(int);  i++)
   {
      if(speed == name_arr[i])
      {             
         cfsetispeed(&options, speed_arr[i]); 
         cfsetospeed(&options, speed_arr[i]); 
         printf("%d-",name_arr[i]);
         speed_sta=1;
	 break; 
      }
   }     
   if(speed_sta==0)
   {
     printf("\nSpeed value[%d] is wrong!\n",speed);
     return (FALSE); 
   }     
   
   //修改控制模式，保证程序不会占用串口
   options.c_cflag |= CLOCAL;
   //修改控制模式，使得能够从串口中读取输入数据
   options.c_cflag |= CREAD;
  
   //设置数据流控制
   switch(flow_ctrl)
   {      
       case 0 ://不使用流控制
              options.c_cflag &= ~CRTSCTS;
              break;        
       case 1 ://使用硬件流控制
              options.c_cflag |= CRTSCTS;
              break;
       case 2 ://使用软件流控制
              options.c_cflag |= IXON | IXOFF | IXANY;
              break;
   }

    //设置校验位
    switch (parity)
    {  
       case 'n':
       case 'N': //无奇偶校验位。
                 options.c_cflag &= ~PARENB; 
                 options.c_iflag &= ~INPCK; 
                 printf("N-");   
                 break; 
       case 'o':  
       case 'O'://设置为奇校验    
                 options.c_cflag |= (PARODD | PARENB); 
                 options.c_iflag |= INPCK;
                 printf("O-");             
                 break; 
       case 'e': 
       case 'E'://设置为偶校验  
                 options.c_cflag |= PARENB;       
                 options.c_cflag &= ~PARODD;       
                 options.c_iflag |= INPCK;
                 printf("E-");       
                 break;
       case 's':
       case 'S': //设置为空格 
                 options.c_cflag &= ~PARENB;
                 options.c_cflag &= ~CSTOPB;
                 printf("S-"); 
                 break; 
        default:  
                 printf("\nParity bit[%c] is not support!\n",parity);    
                 return (FALSE); 
    } 
    //设置数据位
    //屏蔽其他标志位
    options.c_cflag &= ~CSIZE;
    switch (databits)
    {  
       case 5    :
                     options.c_cflag |= CS5;
                     printf("5-");
                     break;
       case 6    :
                     options.c_cflag |= CS6;
                     printf("6-");
                     break;
       case 7    :    
                     options.c_cflag |= CS7;
                     printf("7-");
                     break;
       case 8    :    
                     options.c_cflag |= CS8;
                     printf("8-");
                     break;  
       default   :   
                     printf("\nData bit[%d] is not support\n",databits);
                     return (FALSE); 
    }

    // 设置停止位 
    switch (stopbits)
    {  
       case 1:   
                 options.c_cflag &= ~CSTOPB;                  
                 printf("1\n");
                 break; 
       case 2:   
                 options.c_cflag |= CSTOPB; 
                 printf("2\n");
                 break; 
      
       default:   
                printf("\nStop bit[%d] is not support\n",stopbits); 
                return (FALSE);
    }
   
  //修改输出模式，原始数据输出
  options.c_oflag &= ~OPOST;
  
  options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);//我加的
  //options.c_lflag &= ~(ISIG | ICANON);
   
    //设置等待时间和最小接收字符
    //options.c_cc[VTIME] = 50; /* 读取一个字符等待1*(1/10)s */  
    //options.c_cc[VMIN] = 0; /* 读取字符的最少个数为1 */
   
    //刷清收发缓存区内的数据
    tcflush(fd,TCIFLUSH);
   
    //激活配置 (将修改后的termios数据设置到串口中）
    if (tcsetattr(fd,TCSANOW,&options) != 0)  
    {
       perror("com set error!\n");  
       return (FALSE); 
    }
    return (TRUE); 
}

/*******************************************************************
* 名称：UART0_Recv
* 功能：串口接收(带超时检测功能)，接收的数据保存在输入所指的数组
* 入口参数：  fd:串口文件描述符   
            *rcv_buf:接收到数据后存放的位置
            data_len：需要接收的个数
                     
* 出口返回： >0:实际接收到的个数
           -1：接收超时(接收超时时间为Time_out这个宏定义)
*******************************************************************/
int UART_Recv(int fd, uchar *rcv_buf,int data_len)
{
    int len,fs_sele;
    struct timeval time;
    
    fd_set fs_read;  
    FD_ZERO(&fs_read);
    FD_SET(fd,&fs_read);
   
    time.tv_sec = Time_out;
    time.tv_usec = 0;
   
    
    fs_sele = select(fd+1,&fs_read,NULL,NULL,&time);
    if(fs_sele)
       {
              len = read(fd,rcv_buf,data_len);
	          //printf("len = %d fs_sele = %d\n",len,fs_sele);
              //tcflush( fd, TCIFLUSH );//只接收data_len个数据，没接收完的部分进行清空,不循环读取
              return len;
       }
       else
       {
	      printf("time out!\n");
          return FALSE;
       }     
}




int Lcd_control(int fd, char *cmd)//模式设置
{
  uchar receive_buf[4];
  char *cmdbuf=cmd;
  uchar end_frame[]={0xff,0xff,0xff} , len=0;
  while(*cmdbuf!='\0')
  {
    cmdbuf++;
    len++;
  }
  
  printf("send data is:%s len:%d\n",cmd,len);
  len = write(fd,cmd,len);
  write(fd,end_frame,3);
  if (len > 0 ) 
  {
     printf("send sucess! send_len:%d\n",len);
     //return len;
  }
  else   
  {               
    tcflush(fd,TCOFLUSH);
    return FALSE;
  }
 
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



int Lcd_set_val(int fd, char *cmd, int val)//模式设置
{
  uchar receive_buf[4];
  uchar val_buf[5] , val_len;
  char *cmd_buf = cmd;
  uchar end_frame[]={0xff,0xff,0xff},cmd_len=0;

  val_len = int2str(val,val_buf);//整型转字符型

  while(*cmd_buf!='\0')
  {
    cmd_buf++;
    cmd_len++;
  }
  printf("send data is:%s len:%d\n", cmd, cmd_len);
  cmd_len = write(fd,cmd,cmd_len);//发送控制指令
  if (cmd_len > 0 ) 
  {
     printf("send sucess! send_len:%d\n",cmd_len);
     //return len;
  }
  else   
  {               
    tcflush(fd,TCOFLUSH);
    return FALSE;
  }

  write(fd,val_buf,val_len);
  printf("val_len:%s",val_buf);  

  write(fd,end_frame,3);//帧尾部
 
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
扫描buf中的有效数据帧并将有效数据提取保存到buf1
*p为接收区的地址
*p1为存放区的地址
len为接收区的总长度 
返回值为寄存器的个数
*/  

int get_data(int fd,uchar *p,unsigned short *p1)
{	
    int len;
    uchar save_len,i;
    unsigned short crc_result,ck_crc;
    
    len = UART_Recv(fd,p,80);
    if(len>=0)
    {
	  /*原始数据调试输出
	  printf("raw_len = %d\n",len);
	  printf("the raw_data is:\n");
      for(i=0;i<len;i++)
   	  {
 	    if(receive_buf[i]>>4==0)printf("0x0%X,",receive_buf[i]);//如果数据为单数自动补上0x0
        else 
	    if(i==len-1)printf("0x%x}\n",receive_buf[i]);//探测是否结束
	    else    
        printf("0x%X,",receive_buf[i]); //正常输出  	
      }//*/
      while(len--)
      {
	     if(*p==0xaa && *(p+1)==0x55 && *(p+2)==0xcc)//帧头检测
	     { 
		   crc_result = get_crc(p+3,*(p+5)+3);//数据帧计算CRC
		   ck_crc = *(p+ *(p+5)+7)<<8|*(p+*(p+5)+6);//低高字节CRC转换为整数
		   if(crc_result==ck_crc)//CRC校验比对
		   {   
	          //printf("crc_ok:%d\n",crc_result);
	          //printf("daice:%d\n",ck_crc);
		      save_len=*(p+5)/2;//计算数据对个数
		      //printf("save_len:%d\n",*(p+5));
		      //printf("save_len/2:%d\n",save_len);
	          for(i=0;i<save_len;i++)
	          {
	              *p1=*(p+6)<<8|*(p+6+1);//有效数据合为2个字节的数据（一个寄存器的数据值为2个字节） 
		          //printf("%X  ",*p1);
	              p1++;
	              p=p+2;
	          }
		      break;  
		    }
	      }
          p++;
	    }
        return save_len;
    }
    else
    return FALSE;
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
	  printf("\n\nsend data is:");
	  for(i=0;i<11;i++)

	  printf("%x ",read_buf[i]);
 	  printf("  ");
	 
	  len = write(fd,read_buf,11);
          if (len == 11 ) 
 	  {
             printf("send sucess!\n\n");
             return len;
          }
          else   
          {               
            tcflush(fd,TCOFLUSH);
            return FALSE;
          }
	}
   return TRUE;
}

int char_compare(uchar *ch1,char *ch2)//字符串对比，相同返回0,不同返回-1
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
}
 



int Lcd_touch_read(int fd)
{
  uchar touch_buf[20],buf_len;
  buf_len = UART_Recv(fd,touch_buf,13);
  if(buf_len>0)
  {
    printf("touch_raw_data is:%s\n",touch_buf);
    printf("compare is:%d\n",char_compare(touch_buf,"return_button")); 
    memset(touch_buf,0,20);   
    /*if(char_compare(touch_buf,"sensor_button")==TRUE)return Sensor_button;
    if(char_compare(touch_buf,"control_button")==TRUE)return Control_button;
    if(char_compare(touch_buf,"headctr_button")==TRUE)return Headctr_button;
    if(char_compare(touch_buf,"hardwar_button")==TRUE)return Hardwar_button;
    if(char_compare(touch_buf,"return_button")==TRUE)return Return_button;*/
      
  }
  //printf("chumo chaoshi\n");
  return FALSE; 
}

void *zhukongjieshou(void *fd1)//串口接收的桢解析(线程1)
{
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
           printf("lala\n");
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
    //pthread_t com2_id;

    //*
    fd1 = UART_Open(fd1,"/dev/ttyS0"); //打开串口1，返回文件描述符
    //fd1 = UART_Open(fd1,"/dev/ttyUSB0"); //打开串口1，返回文件描述符
    if(fd1<0)
    { 
      printf("open port1 fail!\n");
      exit(1);
    }

    if(UART_Init(fd1,0,115200,'N',8,1)<0)
    {
       printf("UART_Init1 fail!\n");
       exit(1); 
    }//*/

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

    /*if(pthread_create(&com2_id,NULL,lcdjieshou,NULL)>0)
    {
        printf("the pthread lcdjieshou faill!\n");
        exit(1);
    }*/
//enum window { Sensor_button, Control_button, Headctr_button, Hardwar_button, Return_button};//窗口页面名称
    
     Lcd_control(fd2,"page ultrasonic");
     //sleep(1);
     while (1) //循环读取数据
     { 
        
         send_data(fd1,0,0x81,0x80,12); //主控板数据请求                               		
         //if(real_data[0]!=0&&real_data[1]!=0&&real_data[2]!=0)
        // { 
           Lcd_set_val(fd2,"d0.val=",(real_data[chaosheng1]>>8)*10);
           Lcd_set_val(fd2,"d2.val=",(real_data[chaosheng1]&0x00ff)*10);
           Lcd_set_val(fd2,"d4.val=",(real_data[chaosheng2]>>8)*10);
           Lcd_set_val(fd2,"d6.val=",(real_data[chaosheng2]&0x00ff)*10);
           Lcd_set_val(fd2,"d8.val=",(real_data[chaosheng3]>>8)*10);
           Lcd_set_val(fd2,"da.val=",(real_data[chaosheng3]&0x00ff)*10);
           Lcd_set_val(fd2,"dc.val=",(real_data[chaosheng4]>>8)*10);
           Lcd_set_val(fd2,"de.val=",(real_data[chaosheng4]&0x00ff)*10);
           Lcd_set_val(fd2,"e0.val=",(real_data[chaosheng5]>>8)*10);
           Lcd_set_val(fd2,"e2.val=",(real_data[chaosheng5]&0x00ff)*10);
           Lcd_set_val(fd2,"e4.val=",(real_data[chaosheng6]>>8)*10);
           Lcd_set_val(fd2,"e8.val=",(real_data[chaosheng6]&0x00ff)*10);
           Lcd_set_val(fd2,"hongwaizuo.val=",(real_data[hongwai2]>>8)*2);
           Lcd_set_val(fd2,"hongwaiyou.val=",(real_data[hongwai2]&0x00ff)*2);
           //sleep(1);     
         //}//*/
         //printf("the touch is:%d",Lcd_touch_read(fd2));
         sleep(1);
     }
   UART_Close(fd1);
}


