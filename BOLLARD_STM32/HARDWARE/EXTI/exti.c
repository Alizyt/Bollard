#include "exti.h"
#include "delay.h"
#include "controlfunction.h"  
#include "recordmanage.h"
//#include "timer.h"

_calendar_obj slideCalendarPre1,slideCalendarPre2,slideCalendarPre3;	//�����ṹ��
u8 slideTime=24;//�»���ʱ�������ã�СʱΪ��λ
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

//ControlFunction�жϷ�ʽ��ʼ��
void EXTIX_Init(void)
{
 
 	EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;

    ControlFunction_Init();	 //	�����˿ڳ�ʼ��

  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//ʹ�ܸ��ù���ʱ��

		//GPIOC.5�ж����Լ��жϳ�ʼ������  �����ش���
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource5);
		EXTI_InitStructure.EXTI_Line = EXTI_Line5;//UpperLimit1
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
		EXTI_Init(&EXTI_InitStructure);	 	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���
		
		//GPIOD.8�ж����Լ��жϳ�ʼ������ �����ش��� 
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOD,GPIO_PinSource8);
	 	EXTI_InitStructure.EXTI_Line = EXTI_Line8;//UpperLimit2
  	EXTI_Init(&EXTI_InitStructure);	  //����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���

		//GPIOE.15�ж����Լ��жϳ�ʼ������ �����ش��� 
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource15);
	 	EXTI_InitStructure.EXTI_Line = EXTI_Line15;//UpperLimit3
  	EXTI_Init(&EXTI_InitStructure);	  //����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���

		//GPIOE.2�ж����Լ��жϳ�ʼ������ �½��ش��� 
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource2);
  	EXTI_InitStructure.EXTI_Line=EXTI_Line2;//MXOF
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_Init(&EXTI_InitStructure);	  	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���

		NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;			//ʹ��limit���ڵ��ⲿ�ж�ͨ��
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//��ռ���ȼ�2 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;					//�����ȼ�1 
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure);  	  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

		NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;			//ʹ��Limit���ڵ��ⲿ�ж�ͨ��
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//��ռ���ȼ�2 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;					//�����ȼ�0 
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure);  	  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

		NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;			//ʹ��MXOF���ڵ��ⲿ�ж�ͨ��
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//��ռ���ȼ�2�� 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;					//�����ȼ�2
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure);

		//�»�ʱ�ӳ�ʼ��
		slideCalendarPre1=calendar;
		slideCalendarPre2=calendar;
		slideCalendarPre3=calendar;
}
 
void EXTI9_5_IRQHandler(void)
{
	delay_ms(10);//����
	if(EXTI_GetITStatus(EXTI_Line5)!=RESET)//�ж�LINE5���ж��Ƿ���
	{
		if(BollardControlDown!=ControlEnable)
		{
			//Bollard_Control(Control_Bollard_Up,Bollard1);
			BollardControlUp=ControlEnable;
			controlOn=1;
			controlSource=Bollard1;
			TIM_Cmd(TIM2,ENABLE);
			//SD����¼1��Һѹ���»��¼�
			Error_Event_Save(slideDown,Bollard1);
			//��ǰʱ��-�ϴ��»�ʱ��<�»���ʱ������
			if(Slide_Time_Compare(slideCalendarPre1))
			{
				//����
			}
			slideCalendarPre1=calendar;
		}			
		EXTI_ClearITPendingBit(EXTI_Line5);  //���LINE5�ϵ��жϱ�־λ  
	}
	else if(EXTI_GetITStatus(EXTI_Line8)!=RESET)//�ж�LINE8���ж��Ƿ���
	{
		if(BollardControlDown!=ControlEnable)
		{
			//Bollard_Control(Control_Bollard_Up,Bollard2);
			BollardControlUp=ControlEnable;
			controlOn=1;
			controlSource=Bollard2;
			TIM_Cmd(TIM2,ENABLE);
			//SD����¼2��Һѹ���»��¼�
			Error_Event_Save(slideDown,Bollard2);
			//��ǰʱ��-�ϴ��»�ʱ��<�»���ʱ������
			if(Slide_Time_Compare(slideCalendarPre2))
			{
				//����
			}
			slideCalendarPre2=calendar;
		}		 
		EXTI_ClearITPendingBit(EXTI_Line8);  //���LINE8�ϵ��жϱ�־λ  
	}
}

void EXTI15_10_IRQHandler(void)
{
	delay_ms(10);//����
	if(EXTI_GetITStatus(EXTI_Line15)!=RESET)//�ж�LINE15���ж��Ƿ���
	{
		if(BollardControlDown!=ControlEnable)
		{
			//Bollard_Control(Control_Bollard_Up,Bollard3);
			BollardControlUp=ControlEnable;
			controlOn=1;
			controlSource=Bollard3;
			TIM_Cmd(TIM2,ENABLE);
			//SD����¼3��Һѹ���»��¼�
			Error_Event_Save(slideDown,Bollard3);
			//��ǰʱ��-�ϴ��»�ʱ��<�»���ʱ������
			if(Slide_Time_Compare(slideCalendarPre3))
			{
				//����
			}
			slideCalendarPre3=calendar;
		}			
		EXTI_ClearITPendingBit(EXTI_Line15);  //���LINE15�ϵ��жϱ�־λ  
	}
}

//�ⲿ�ж�2�������
void EXTI2_IRQHandler(void)
{
	delay_ms(10);//����
	if(MXOF==StatusTrue) 	  //MXOF
	{
		//�ϵ磬��������
		Power_Event_Save(powerOff);
	}		 
	EXTI_ClearITPendingBit(EXTI_Line2);  //���LINE2�ϵ��жϱ�־λ  
}

