#ifndef _TEST_H
#define _TEST_H
//共享的全局变量
unsigned char lcd_status;//lcd的窗口切换标志位
enum reg_ename {banben,zuolicheng=3,youlicheng,chaokuandaiX,chaokuandaiY,chaosheng1=11,chaosheng2,chaosheng3,chaosheng4,chaosheng5,chaosheng6,hongwai1=21,hongwai2,dianliang1=25,dianliang2}; 
enum window { main_window=2, Sensor_window, Control_window, Headctr_window, versions_window, 
              Return_button,
              hbiaoding, hzuo, hyou, hzhong, 
              zbiaoding, zzuo, zyou, zzhong,
              ybiaoding, yzuo, yyou, yzhong,
              led00,led01,
              led10,led11,
              alianxu0,alianxu1};//窗口页面名称

int send_data(int fd,uchar rw,uchar device_n,uchar reg_n,uchar reg_mun);
int motor_contrl_send(int fd, uchar device_n,uchar ctrl_mode,uchar speed,unsigned int weizhi);
int led_contrl_send(int fd, uchar device_n,uchar led1_cycle,uchar led2_cycle,uchar liangdu);
void  delayms(unsigned int ms);
#endif
