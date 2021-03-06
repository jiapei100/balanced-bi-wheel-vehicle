#include "delay.h"
#include "blt.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	 
/***************************************************************
Copyright (C), 2013-2023, YFRobot.
www.yfrobot.com
File：串口2驱动代码(PA2/U2_TX;PA3/U2_RX)
Author：aosini    Version:1.0     Data:2014/05/28
Description:此函数对原子的串口驱动代买进行了修改，在此感谢正点原子@ALIENTEK分享
            硬件与BC04的接线方式，PA2接蓝牙RXD；PA3接TXD。
***************************************************************/  

//串口发送缓存区 	
__align(8) u8 USART2_TX_BUF[USART2_MAX_SEND_LEN]; 	//发送缓冲,最大USART2_MAX_SEND_LEN字节
#ifdef USART2_RX_EN   								//如果使能了接收   	  
//串口接收缓存区 	
u8 USART2_RX_BUF[USART2_MAX_RECV_LEN]; 				//接收缓冲,最大USART2_MAX_RECV_LEN个字节.
int s=9;
int num=0;
//通过判断接收连续2个字符之间的时间差不大于10ms来决定是不是一次连续的数据.
//如果2个字符接收间隔超过10ms,则认为不是1次连续数据.也就是超过10ms没有接收到
//任何数据,则表示此次接收完毕.
//接收到的数据状态
//[15]:0,没有接收到数据;1,接收到了一批数据.
//[14:0]:接收到的数据长度
u16 USART2_RX_STA=0;  
u8 reclen=0;
void USART2_IRQHandler(void)
{
	u8 res;	int i;    
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)//接收到数据
	{	
		res=USART_ReceiveData(USART2);
		USART2_RX_BUF[USART2_RX_STA++]=res;
 		reclen = USART2_RX_STA&0X7FFF;	//得到数据长度	 	//加入结束符
	  USART_SendData(USART2,USART2_RX_BUF[reclen-1]);
	  while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET); 

		//判断接收的数据，并做出相应动作
			if(USART2_RX_BUF[0] == '0')       {s=0;}	
			else if(USART2_RX_BUF[0] == '1')  {s=1;}
			else if(USART2_RX_BUF[0] == '2')  {s=2;}
			else if(USART2_RX_BUF[0] == '3')  {s=3;}
			else if(USART2_RX_BUF[0] == '4')  {s=4;}
			else if(USART2_RX_BUF[0] =='5')   {s=5;}
			else if(USART2_RX_BUF[0] == '6')  {s=6;}
			else if(USART2_RX_BUF[0] == '7')  {s=7;}
			else if(USART2_RX_BUF[0] == '8')  {s=8;}
			else if(USART2_RX_BUF[0] == '9')  {s=10;}
			//else if(USART2_RX_BUF[0] == '10')  {s=12;}
 			USART2_RX_STA=0;	 	
		}	
		//res=USART2->DR; 			 
//		//if(USART2_RX_STA<USART2_MAX_RECV_LEN)		//还可以接收数据
//		{
//			//TIM1->CNT=0;         					//计数器清空
//			//if(USART2_RX_STA==0)TIM1_Set(1);	 	//使能定时器4的中断 
//			USART2_RX_BUF[USART2_RX_STA++]=res;		//记录接收到的值	 
//		}else 
//		{
//			USART2_RX_STA|=1<<15;					//强制标记接收完成
//		} 
	}  											 
//初始化IO 串口2
//pclk1:PCLK1时钟频率(Mhz)
//bound:波特率	  
void USART2_Init(u32 bound)
{  	 		 
//	RCC->APB2ENR|=1<<8;   	//使能PORTG口时钟  
// 	GPIOG->CRH&=0XFFFFFF0F;	//IO状态设置
//	GPIOG->CRH|=0X00000030;	//IO状态设置
//	RCC->APB2ENR|=1<<2;   	//使能PORTA口时钟  
//	GPIOA->CRL&=0XFFFF00FF;	//IO状态设置
//	GPIOA->CRL|=0X00008B00;	//IO状态设置	 
//	RCC->APB1ENR|=1<<17;  	//使能串口时钟 	 
//	RCC->APB1RSTR|=1<<17;   //复位串口2
//	RCC->APB1RSTR&=~(1<<17);//停止复位	   	   
//	//波特率设置
// 	USART2->BRR=(pclk1*1000000)/(bound);// 波特率设置	 
//	USART2->CR1|=0X200C;  	//1位停止,无校验位.
//	USART2->CR3=1<<7;   	//使能串口2的DMA发送
//	UART_DMA_Config(DMA1_Channel7,(u32)&USART2->DR,(u32)USART2_TX_BUF);//DMA1通道7,外设为串口2,存储器为USART2_TX_BUF 
	
	//pa2 pa3
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
#ifdef USART2_RX_EN
		NVIC_InitTypeDef NVIC_InitStructure;
#endif
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	//TX
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);     	
	//RX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);   
	
	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &USART_InitStructure); 
  USART_Cmd(USART2, ENABLE);
	
#ifdef USART2_RX_EN		  	//如果使能了接收
//	//使能接收中断
//	USART2->CR1|=1<<8;    	//PE中断使能
//	USART2->CR1|=1<<5;    	//接收缓冲区非空中断使能	    	
//	MY_NVIC_Init(2,3,USART2_IRQn,2);//组2，最低优先级 
//	//TIM1_Init(99,7199);		//10ms中断
//	USART2_RX_STA=0;		//清零
//	//TIM1_Set(0);			//关闭定时器4
USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启相关中断

	//Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;//串口2中断通道   
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;//抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化NVIC寄存器

#endif										  	
}
//串口2,printf 函数
//确保一次发送数据不超过USART2_MAX_SEND_LEN字节
//void u2_printf(char* fmt,...)  
//{  
//	va_list ap;
//	va_start(ap,fmt);
//	vsprintf((char*)USART2_TX_BUF,fmt,ap);
//	va_end(ap);
//	while(DMA1_Channel7->CNDTR!=0);	//等待通道7传输完成   
//	UART_DMA_Enable(DMA1_Channel7,strlen((const char*)USART2_TX_BUF)); 	//通过dma发送出去
//}
void USART2_SendString(char *fmt, ...)
 {
	char buffer[USART2_MAX_SEND_LEN+1];  // 
	u8 i = 0;
	
	va_list arg_ptr;
	va_start(arg_ptr, fmt);  
	vsnprintf(buffer, USART2_MAX_SEND_LEN+1, fmt, arg_ptr);
	USART_ClearFlag(USART2,USART_FLAG_TXE);
	while ((i < USART2_MAX_SEND_LEN) && buffer[i])
	{
		if(buffer[i] == '\n'){
        USART_SendData(USART2,(u8)'\r');
        while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET); 
        USART_SendData(USART2,(u8)buffer[i++]);
        }else{
	    USART_SendData(USART2, (u8) buffer[i++]);
        }
        while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET); 
	}
	va_end(arg_ptr);
 }


//定时器1中断服务程序		    
//void TIM1_UP_IRQHandler(void)
//{ 	
//	if(TIM1->SR&0X01)//是更新中断
//	{	 			   
//		USART2_RX_STA|=1<<15;	//标记接收完成
//		TIM1->SR&=~(1<<0);		//清除中断标志位		   
//		TIM1_Set(0);			//关闭TIM1 
//	}	    
//}
//设置TIM1的开关
//sta:0，关闭;1,开启;
//void TIM1_Set(u8 sta)
//{
//	if(sta)
//	{
//    	TIM1->CNT=0;         //计数器清空
//		TIM1->CR1|=1<<0;     //使能定时器1
//	}else TIM1->CR1&=~(1<<0);//关闭定时器1	   
//}
//通用定时器中断初始化
//这里始终选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数		 
//void TIM1_Init(u16 arr,u16 psc)
//{
//	RCC->APB2ENR|=1<<11;	//TIM1时钟使能    
// 	TIM1->ARR=arr;  	//设定计数器自动重装值   
//	TIM1->PSC=psc;  	//预分频器
// 	TIM1->DIER|=1<<0;   //允许更新中断				
// 	TIM1->CR1|=0x01;  	//使能定时器1	  	   
//   	MY_NVIC_Init(1,3,TIM1_UP_IRQn,2);//抢占2，子优先级3，组2	在2中优先级最低								 
//}
#endif		 
///////////////////////////////////////USART2 DMA发送配置部分//////////////////////////////////	   		    
//DMA1的各通道配置
//这里的传输形式是固定的,这点要根据不同的情况来修改
//从存储器->外设模式/8位数据宽度/存储器增量模式
//DMA_CHx:DMA通道CHx
//cpar:外设地址
//cmar:存储器地址    
//void UART_DMA_Config(DMA_Channel_TypeDef*DMA_CHx,u32 cpar,u32 cmar)
//{
// 	RCC->AHBENR|=1<<0;			//开启DMA1时钟
//	delay_us(5);
//	DMA_CHx->CPAR=cpar; 		//DMA1 外设地址 
//	DMA_CHx->CMAR=cmar; 		//DMA1,存储器地址	 
//	DMA_CHx->CCR=0X00000000;	//复位
//	DMA_CHx->CCR|=1<<4;  		//从存储器读
//	DMA_CHx->CCR|=0<<5;  		//普通模式
//	DMA_CHx->CCR|=0<<6;  		//外设地址非增量模式
//	DMA_CHx->CCR|=1<<7;  		//存储器增量模式
//	DMA_CHx->CCR|=0<<8;  		//外设数据宽度为8位
//	DMA_CHx->CCR|=0<<10; 		//存储器数据宽度8位
//	DMA_CHx->CCR|=1<<12; 		//中等优先级
//	DMA_CHx->CCR|=0<<14; 		//非存储器到存储器模式		  	
//} 
////开启一次DMA传输
//void UART_DMA_Enable(DMA_Channel_TypeDef*DMA_CHx,u8 len)
//{
//	DMA_CHx->CCR&=~(1<<0);       //关闭DMA传输 
//	DMA_CHx->CNDTR=len;          //DMA1,传输数据量 
//	DMA_CHx->CCR|=1<<0;          //开启DMA传输
//}	   
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 									 
//函数名：myatof
//功能：把字符串转化为 double 浮点型
//名字来源：my array to floating point number
//函数说明：接收一个字符串判断第一个字符好，没有符号默认为正值，然后对剩下字符进行转换，
//遇到\0结束，最后返回一个double
float myatof(const char *sptr)
{
    float temp = 10;
	char ispnum = true;
	float ans = 0;
	if(*sptr == '-')  //判断是否是负数
	{
	    ispnum = false;
		sptr++;
	}
	else if(*sptr == '+') //判断是否为正数
	{
	    sptr++;
	}
	while(*sptr != '\0')//寻找小数点之前的数
	{
	    if(*sptr == '.') {sptr++; break;}//检测到‘0’ 则结束循环
		ans = ans*10 + (*sptr - '0');
		sptr++;
	}
	while(*sptr != '\0')
	{
	    ans = ans + (*sptr - '0')/temp;
		temp *= 10;
		sptr++;
	}
	if(ispnum)return ans;
	else return ans*(-1); 

}























