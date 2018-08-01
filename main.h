#ifndef _MAIN_H 
#define _MAIN_H
#define uchar unsigned char
//共享的全局变量
enum reg_ename {banben,zuolicheng=3,youlicheng,chaokuandaiX,chaokuandaiY,chaosheng1=11,chaosheng2,chaosheng3,chaosheng4,chaosheng5,chaosheng6,hongwai1=21,hongwai2,dianliang1=25,dianliang2}; 
enum window { main_window=2, Sensor_window, Control_window, Headctr_window, versions_window, 
              Return_button,
              hbiaoding, hzuo, hyou, hzhong, 
              zbiaoding, zzuo, zyou, zzhong,
              ybiaoding, yzuo, yyou, yzhong,
              led00,led01,
              led10,led11,
              alianxu0,alianxu1,
              auto_charge_on,auto_charge_off,
	            go_up,go_down,go_left,go_right,go_stop,
             };//窗口页面名称
void motor_contrl_send(uchar device_n,uchar ctrl_mode,uchar speed,unsigned int weizhi);//舵机控制
void	led_contrl_send(uchar device_n,uchar led1_cycle,uchar led2_cycle,uchar liangdu);//LED控制
void main_board_contrl_send(uchar run_mode,uchar left_speed,uchar right_speed);//主控板多写
#endif
