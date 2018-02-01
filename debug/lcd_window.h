#ifndef  _LCD_WINDOW_H
#define  LCD_WINDOW_H

int Lcd_control(int fd, char *cmd);//模式设置
int Lcd_set_val(int fd, char *cmd, int val);//模式设置

int ultrasonic_window(int fd,unsigned short *real_data);
int version_window(int fd0,int fd,unsigned short *real_data);
#endif

