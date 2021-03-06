#include "timer.h"
#include "init.h"
//通用定时器中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器3!
float a=0;
void TIM3_Init(u16 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能

	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 计数到5000为500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(  //使能或者失能指定的TIM中断
		TIM3, //TIM2
		TIM_IT_Update ,
		ENABLE  //使能
		);
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

	TIM_Cmd(TIM3, ENABLE);  //使能TIMx外设
							 
}
u8 interrupt_flag=0;  //周期中断函数分为四个阶段
u8 speed_control_flag=0;//
void TIM3_IRQHandler(void)   //TIM3中断
{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  //清标志位
		interrupt_flag++;
	
		if(interrupt_flag==1)
		{
 			
			Accel_x = GetData_GYRO(ACCEL_XOUT_H);
			Gyro_y =  GetData_GYRO(GYRO_YOUT_H);	
			Angle_Calculate();
			Angle_Control();
			
			
		}
		else if(interrupt_flag==2)
		{//速度控制,暂时用不到，先用来进行小灯控制
			speed_control_flag++;
			if(speed_control_flag>=33)
			{		speed_control_flag = 0;
				
//					Accel_x = GetData_GYRO(ACCEL_XOUT_H);					
//					Gyro_y =  GetData_GYRO(GYRO_YOUT_H);	
//					printf("\r\n%f\t   %f\t  \r\n",Accel_x, Gyro_y);
					if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_0)==Bit_RESET)  
					{  
						GPIO_SetBits(GPIOC,GPIO_Pin_0);  
					}else
					{  
						GPIO_ResetBits(GPIOC,GPIO_Pin_0);  
					} 
			}
		}
		else if(interrupt_flag==3)
		{
			interrupt_flag = 0;
		
		}
		switch(s)
		{
			case 0:
	Set_Motor_Pwm();
		break;
		case 1 :
			Set_control_Pwm(250,0.0);

		num=2;
//		testleft(30);
//		Set_Motor_Pwm();
//		 read();
//		s=9;
			break;
		
		case 2 :
	
		Set_control_Pwm(0.0,900);
		
		num=2;
		break;
		case 3 :
		testright(30);
		Set_Motor_Pwm();
		s=9;
	
		break;
	  case 4 :
    testright(-30);
		Set_Motor_Pwm();
		s=9;

		break;
		 case 5 :
			 	a-=10;
Set_control_Pwm(a,0.0);
			s=11;
     read();
		 USART2_SendString("a:%f\n",a);
		Set_Motor_Pwm();

		 break;
		 case 6 :
		testsetp(10);
		Set_Motor_Pwm();
		s=9;
	
			break;
		
		case 7 :
		testsetp(-10);
		Set_Motor_Pwm();
		s=9;
		break;
		case 8 :
		testsetd(0.05);
		Set_Motor_Pwm();
		s=9;
		
		break;
	  case 10 :
    testsetd(-0.05);
		Set_Motor_Pwm();
		s=9;
		
		break;
		case 11 :
   
		Set_control_Pwm(a,0.0);

		break;
	default:
		Set_Motor_Pwm();
	num=3;
	//Led_RollShow(data);
		break;
		
		
		
		}
}












