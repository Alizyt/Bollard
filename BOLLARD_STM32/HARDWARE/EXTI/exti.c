#include "exti.h"
#include "delay.h"
#include "controlfunction.h"  
#include "recordmanage.h"
//#include "timer.h"

_calendar_obj slideCalendarPre1,slideCalendarPre2,slideCalendarPre3;	//日历结构体
u8 slideTime=24;//下滑延时，可设置，小时为单位
extern u8 controlOn;
extern eBollardControlSource controlSource;
extern u8 maxSliceCnt;

u8 Slide_Time_Compare(_calendar_obj comp)
{
	if((calendar.w_year>comp.w_year)||(calendar.w_month>comp.w_month)||(calendar.w_date-comp.w_date>1))
		return 0;
	else if(calendar.w_date-comp.w_date==1)
	{
		if(calendar.hour+24-comp.hour>slideTime)
			return 0;
		else
			return 1;
	}
	else if(calendar.hour-comp.hour>slideTime)
		return 0;
	else
		return 1;
}

//ControlFunction中断方式初始化
void EXTIX_Init(void)
{
 
 	EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;

    ControlFunction_Init();	 //	按键端口初始化

  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//使能复用功能时钟

		//GPIOC.5中断线以及中断初始化配置  上升沿触发
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource5);
		EXTI_InitStructure.EXTI_Line = EXTI_Line5;//UpperLimit1
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
		EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
		
		//GPIOD.8中断线以及中断初始化配置 上升沿触发 
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOD,GPIO_PinSource8);
	 	EXTI_InitStructure.EXTI_Line = EXTI_Line8;//UpperLimit2
  	EXTI_Init(&EXTI_InitStructure);	  //根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

		//GPIOE.15中断线以及中断初始化配置 上升沿触发 
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource15);
	 	EXTI_InitStructure.EXTI_Line = EXTI_Line15;//UpperLimit3
  	EXTI_Init(&EXTI_InitStructure);	  //根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

		//GPIOE.2中断线以及中断初始化配置 下降沿触发 
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource2);
  	EXTI_InitStructure.EXTI_Line=EXTI_Line2;//MXOF
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_Init(&EXTI_InitStructure);	  	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

		NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;			//使能limit所在的外部中断通道
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//抢占优先级2 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;					//子优先级1 
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure);  	  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

		NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;			//使能Limit所在的外部中断通道
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//抢占优先级2 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;					//子优先级0 
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure);  	  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

		NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;			//使能MXOF所在的外部中断通道
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//抢占优先级2， 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;					//子优先级2
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure);

		//下滑时钟初始化
		slideCalendarPre1=calendar;
		slideCalendarPre2=calendar;
		slideCalendarPre3=calendar;
}
 
void EXTI9_5_IRQHandler(void)
{
	delay_ms(10);//消抖
	if(EXTI_GetITStatus(EXTI_Line5)!=RESET)//判断LINE5上中断是否发生
	{
		if(BollardControlDown!=ControlEnable)
		{
			//Bollard_Control(Control_Bollard_Up,Bollard1);
			BollardControlUp=ControlEnable;
			controlOn=1;
			controlSource=Bollard1;
			TIM_Cmd(TIM2,ENABLE);
			//SD卡记录1号液压柱下滑事件
			Error_Event_Save(slideDown,Bollard1);
			//当前时间-上次下滑时间<下滑延时，警报
			if(Slide_Time_Compare(slideCalendarPre1))
			{
				//警报
			}
			slideCalendarPre1=calendar;
		}			
		EXTI_ClearITPendingBit(EXTI_Line5);  //清除LINE5上的中断标志位  
	}
	else if(EXTI_GetITStatus(EXTI_Line8)!=RESET)//判断LINE8上中断是否发生
	{
		if(BollardControlDown!=ControlEnable)
		{
			//Bollard_Control(Control_Bollard_Up,Bollard2);
			BollardControlUp=ControlEnable;
			controlOn=1;
			controlSource=Bollard2;
			TIM_Cmd(TIM2,ENABLE);
			//SD卡记录2号液压柱下滑事件
			Error_Event_Save(slideDown,Bollard2);
			//当前时间-上次下滑时间<下滑延时，警报
			if(Slide_Time_Compare(slideCalendarPre2))
			{
				//警报
			}
			slideCalendarPre2=calendar;
		}		 
		EXTI_ClearITPendingBit(EXTI_Line8);  //清除LINE8上的中断标志位  
	}
}

void EXTI15_10_IRQHandler(void)
{
	delay_ms(10);//消抖
	if(EXTI_GetITStatus(EXTI_Line15)!=RESET)//判断LINE15上中断是否发生
	{
		if(BollardControlDown!=ControlEnable)
		{
			//Bollard_Control(Control_Bollard_Up,Bollard3);
			BollardControlUp=ControlEnable;
			controlOn=1;
			controlSource=Bollard3;
			TIM_Cmd(TIM2,ENABLE);
			//SD卡记录3号液压柱下滑事件
			Error_Event_Save(slideDown,Bollard3);
			//当前时间-上次下滑时间<下滑延时，警报
			if(Slide_Time_Compare(slideCalendarPre3))
			{
				//警报
			}
			slideCalendarPre3=calendar;
		}			
		EXTI_ClearITPendingBit(EXTI_Line15);  //清除LINE15上的中断标志位  
	}
}

//外部中断2服务程序
void EXTI2_IRQHandler(void)
{
	delay_ms(10);//消抖
	if(MXOF==StatusTrue) 	  //MXOF
	{
		//断电，保存数据
		Power_Event_Save(powerOff);
	}		 
	EXTI_ClearITPendingBit(EXTI_Line2);  //清除LINE2上的中断标志位  
}

