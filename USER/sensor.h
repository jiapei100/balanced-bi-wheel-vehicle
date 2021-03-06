#ifndef _SENSOR_H
#define _SENSOR_H

extern float Gyro_y;        //Y轴陀螺仪数据暂存
extern float Angle_gy;      //由角速度计算的倾斜角度
extern float Accel_x;	     //X轴加速度值暂存
extern float Angle_ax;      //由加速度计算的倾斜角度
extern float Angle;         //最终测量角度


void Angle_Calculate(void);
void Angle_Control(void);
void Set_Motor_Pwm(void);//电机控制
void Set_control_Pwm(float g_Speed_Control_Out,float g_Direction_Control_Out);
//void Set_Motor_Pwm(float a);
void testleft(float a);
void testright(float a);
void read(void);
void testsetp(float a);
void testsetd(float a);
#endif
