#include "STC12C5A.h"
#include "main.h"
#include "lcd_window.h"
#include "uart.h"
#include <intrins.h>  
#define uchar unsigned char  
#define uint  unsigned int 
#define S2RI 0X01
#define S2TI 0X02

uchar lcd_status=0;//LCD�����л�״̬

uchar com2_data[5]={0};
uchar b=0;
uchar data_buf[100]={0},start_recive=0,start_fram=0,frame_len=0,data_len=0;
uchar data_buf1[20]={0},start_recive1=0,start_fram1=0,frame_len1=0,data_len1=0;

unsigned short real_data[50]={0};//���ذ���ս�����ŵ�
unsigned short real_data1[1]={0};//LCDģ����ս�����Ż���(ֻ����һ���Ĵ�������)

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
	IAP_CONTR=0X60;//�Զ�����ϵͳ
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
void motor_contrl_send(uchar device_n,uchar ctrl_mode,uchar speed,unsigned int weizhi)//�������
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

void led_contrl_send(uchar device_n,uchar led1_cycle,uchar led2_cycle,uchar liangdu)//LED����
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

void main_board_contrl_send(uchar run_mode,uchar left_speed,uchar right_speed)//���ذ��д
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

void Main(void)  
{  
  UART1_Init();
  UART2_Init();

  Lcd_control("page main"); 
  //Lcd_control("page ultrasonic"); 	
  while(1)
  { 
          if(lcd_status==Sensor_window)
          {   						
              Lcd_control("page ultrasonic");
						
						  TH1 = 0xFD;		
              TL1 = TH1;	//115200						
              while (1) 
  						{ 
                if(ultrasonic_window(real_data)<0) break;
                delayms(250);
              }
           }
          if(lcd_status==Control_window)//�����˿���
          {   						
              Lcd_control("page robot control");
						
						  TH1 = 0xFD;		
              TL1 = TH1;	//115200						
              while (1) 
  						{ 
                if(robot_ctrl_window(real_data)<0) break;
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
					 
           if(lcd_status==Headctr_window)
           {   
              Lcd_control("page head contrl");
              TH1 = 0xf7;	
	            TL1 = TH1;	//38400������ 						 
              motor_contrl_send(0x01,0x00,0,0);//��ʼ�����
              delayms(30);
              motor_contrl_send(0x06,0x00,0,0);//��ʼ�����
              delayms(30);  
              motor_contrl_send(0x07,0x00,0,0);//��ʼ�����     
              while (1) //ѭ����ȡ����
              {         
                if(motor_ctrl_window(real_data)<0) break;
                //delayms(5);
              }
            } 					 
        delayms(1);
        
   }//end while(1)
}
	
void zhukongjieshou(void)	interrupt 4  
{
  uchar i,j;
  unsigned short crc_result,ck_crc;            
  if(RI == 1)   //��Ӳ�����յ�һ������ʱ��RI����λ	
  {
    RI = 0; 
    data_buf[data_len]=SBUF;
    if(start_recive==0)
    {
      if(data_len==0&&data_buf[data_len]==0xaa)
      {  
        data_len=1; 
      }      
	  else if(data_len==1&&data_buf[data_len]==0xaa)//���´δ����Ļ���֡ͷ�ٴ���aa��ʼ�ж�֡
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
      if(start_fram==1)//���ճ�������
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
           crc_result = get_crc(&data_buf[3],data_buf[5]+3);//����֡����CRC
           ck_crc = data_buf[ (data_buf[5]+7)]<<8 | data_buf[ (data_buf[5]+6) ];//�͸��ֽ�CRCת��Ϊ����
	       if(crc_result==ck_crc)//CRCУ��ȶ�
	       {  
	         for(i=0;i<(data_buf[5]/2);i++)//�������ݶԸ���
	         {
	           real_data[i]=data_buf[6+j]<<8| data_buf[7+j];//��Ч���ݺ�Ϊ2���ֽڵ����ݣ�һ���Ĵ���������ֵΪ2���ֽڣ� 
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

void lcdjieshou(void) interrupt 8 using 1//�����ַ�������
{ 

 uchar i,j;
 unsigned short crc_result,ck_crc;

if(S2CON&S2RI)//�����ݴ�����Ӳ����λS2RI
{
 S2CON&=~S2RI;	//�����λ���Ա���һ���ݴ���		 
   data_buf1[data_len1]=S2BUF; 
     if(start_recive1==0)
     {
       if(data_len1==0&&data_buf1[data_len1]==0xaa)
       {  
         data_len1=1;         
       }      
	   else if(data_len1==1&&data_buf1[data_len1]==0xaa)//���´δ����Ļ���֡ͷ�ٴ���aa��ʼ�ж�֡
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
        
       if(start_fram1==1)//���ճ�������
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
           crc_result = get_crc3(&data_buf1[3],data_buf1[5]+3);//����֡����CRC
           ck_crc = data_buf1[ (data_buf1[5]+7)]<<8 | data_buf1[ (data_buf1[5]+6) ];//�͸��ֽ�CRCת��Ϊ����
	       if(crc_result==ck_crc)//CRCУ��ȶ�
	       {  
	         for(i=0;i<(data_buf1[5]/2);i++)//�������ݶԸ���
	         {
	           real_data1[i]=data_buf1[6+j]<<8| data_buf1[7+j];//��Ч���ݺ�Ϊ2���ֽڵ����ݣ�һ���Ĵ���������ֵΪ2���ֽڣ� 
	           j=j+2;
	         }	         
         }            
           start_recive1=0;
           start_fram1=0;
		       data_len1=0;
		       frame_len1=0;
		       j=0; i=0;
     
           //ҳ�洰�ڲ���
           if(real_data1[0]==0x0000)
           {
             ////printf("Control_window\n");
             lcd_status=Control_window;
           }
           if(real_data1[0]==0x0001)
           {
             ////printf("Sensor_window\n");
             lcd_status=Sensor_window;
           }
           if(real_data1[0]==0x0002)
           {
             ////printf("Headctr_window\n");
             lcd_status=Headctr_window;
           }
           if(real_data1[0]==0x0003)
           {
             ////printf("versions_window\n");
             lcd_status=versions_window;
           } 
           if(real_data1[0]==0x00bc)
           {
             ////printf("back button\n");
             lcd_status=Return_button;
           }
           //ͷ�����ڲ���
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

           //���ִ��ڲ���
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

           //���ִ��ڲ���
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

           //LED�ư���Ʋ��� 
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

           //�������Ʋ��� 
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
			     
					 //�Զ���粿��
					 if(real_data1[0]==0xc100)//�رճ��
           {
             lcd_status=auto_charge_off;
           } 
           if(real_data1[0]==0xc101)//�������
           {
             lcd_status=auto_charge_on;
           }
					 
					 //�����˿������߲���
						if(real_data1[0]==0xd1d1)//��
						{	
             lcd_status=go_up;
            } 
           if(real_data1[0]==0xd2d2)//��
           {
             lcd_status=go_down;
           }
						if(real_data1[0]==0xd3d3)//��
           {
             lcd_status=go_left;
           } 
           if(real_data1[0]==0xd4d4)//��
           {
             lcd_status=go_right;
           }
           if(real_data1[0]==0xd0d0)//ͣ
           {
             lcd_status=go_stop;
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