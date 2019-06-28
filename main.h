#ifndef _MAIN_H 
#define _MAIN_H
#define uchar unsigned char
//共享的全局变量
enum reg_ename {banben,zuolicheng=3,youlicheng,chaokuandaiX,chaokuandaiY,chaosheng1=11,chaosheng2,chaosheng3,chaosheng4,chaosheng5,chaosheng6,hongwai1=21,hongwai2,dianliang1=25,dianliang2}; 
//从0x80开始

enum window { main_window=2, 
	            Sensor_window, 
	            Sensor_window1, 	
	            Control_window,
	            Control_window1,	
	            Headctr_window,
	            Headctr_window1,	
	            versions_window,
	            versions_window1, 	
	            duojixianzhi_window,
							Biaoqing_window,
              Return_button,
              Return_button1,	
             	hbiaoding, hzuo, hyou, hzhong, 
              zbiaoding, zzuo, zyou, zzhong,
              ybiaoding, yzuo, yyou, yzhong,
              led00,led01,
              led10,led11,
              alianxu0,alianxu1,
	
              auto_charge_on,auto_charge_off,
	            go_up,go_down,go_left,go_right,go_stop,	
	
							zhengchang,aixin,chongdian,daohang,
							
	            set_limit_value,
							
							feng_de_kong,
							yi_ban_qudong
             };//窗口页面名称
void motor_contrl_send(uchar device_n,uchar ctrl_mode,uchar speed,unsigned int weizhi);//舵机控制
void motor_contrl_send1(uchar reg_addr ,uchar ctrl_mode,uchar speed,unsigned int weizhi);//舵机控制
void led_contrl_send(uchar device_n,uchar led1_cycle,uchar led2_cycle,uchar liangdu);//LED控制
void led_contrl_send1(uchar power,uchar red,uchar green,uchar blue);//LED控制
void main_board_single_write(uchar reg,unsigned short reg_data);//主控板单写
void main_board_contrl_send(uchar run_mode,uchar left_speed,uchar right_speed);//主控板多写
void main_board_contrl_send1(uchar run_mode,unsigned short left_speed,unsigned short right_speed);//主控板多写
void limit_current_contrl_send(uchar device_n ,uchar limit_current);//限制电流发送
void limit_value_contrl_send(uchar device_n ,uchar limit_up_location,uchar limit_down_location);//限位值发送
void biao_qing_contrl_send(unsigned int page);//表情控制发送
						 
#endif
