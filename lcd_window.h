#ifndef  _LCD_WINDOW_H
#define  _LCD_WINDOW_H
#define uchar unsigned char
extern void send_data(uchar device_n,uchar reg_n,uchar reg_mun); //主体在main.c中
extern void  delayms(uchar ms); //主体在main.c中
extern uchar lcd_status; //lcd的窗口切换标志位，主体在main.c中

int Lcd_control(char *cmd);//向LCD发送控制命令
int Lcd_set_val(char *cmd, int val);//向LCD对象发送数字
int Lcd_set_txt(char *cmd,unsigned short *real_data);//向LCD对象发送字符串
int ultrasonic_window(unsigned short *real_data);//刷新传感器窗口
int version_window(unsigned short *real_data);//刷新硬件版本窗口
#endif

