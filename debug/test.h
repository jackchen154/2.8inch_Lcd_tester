#ifndef _TEST_H
#define _TEST_H
//共享的全局变量
unsigned char lcd_status;//lcd的窗口切换标志位
enum reg_ename {banben,zuolicheng=3,youlicheng,chaokuandaiX,chaokuandaiY,chaosheng1=11,chaosheng2,chaosheng3,chaosheng4,chaosheng5,chaosheng6,hongwai1=21,hongwai2,dianliang1=25,dianliang2}; 
enum window { main_window, Sensor_window, Control_window, Headctr_window, versions_window, Return_button};//窗口页面名称
int send_data(int fd,uchar rw,uchar device_n,uchar reg_n,uchar reg_mun);
void  delayms(unsigned int ms);
#endif
