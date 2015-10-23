#include "timer.h"
#include "led.h" 
#include "delay.h"
#include "controlfunction.h"
#include "recordmanage.h"

u8 TimeOut=0;
u32 timeSliceCnt=0,maxSliceCnt=0;
u8 alarmOn,controlOn;

extern u8 bottonPressed;//bottonValue,bollardControlType;
extern eBollardBottonValue bottonValue;
extern eBollardControlType bollardControlType;
extern u8 remoteControl;
extern eBollardRemoteValue remoteValue;
extern u8 limitReach;//limitValue,
extern eLimitValue limitValue;
extern u8 statusChange;//bollardStatus,
extern eBollardStatus bollardStatus;
extern u8 cascadeChange;//cascadeConnection,
extern eCascadeConnection cascadeConnectionAft;
//extern u8 coilTrigger;
extern eGroundCoilStatus groundCoilStatus;

u8 bottonFlag=0,remoteFlag=0,cascadeFlag=0;
eBollardControlSource controlSource;

//ͨ�ö�ʱ��2�жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
void TIM2_Int_Init(u16 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //ʱ��ʹ��
	
	//��ʱ��TIM3��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	//TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�

	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���

	TIM_Cmd(TIM2,DISABLE);  //ʹ��TIMx
	TIM_SetCounter(TIM2,0);//
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update  );  //���TIMx�����жϱ�־ 
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�	
}

void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  //���TIM2�����жϷ������
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  //���TIMx�����жϱ�־ 	
		
		if(controlOn)
		{
			switch(controlSource)
			{
				case Botton:
					switch(bollardControlType)
					{
						case Control_Bollard_Up:
							BollardControlUp=ControlDisable;
							//��������ʱ��¼
							Control_Event_Save(Control_Bollard_Up,UpTimeout,controlSource);
							break;
						case Control_Bollard_Down:
							BollardControlDown=ControlDisable;
							//���½���ʱ��¼
							Control_Event_Save(Control_Bollard_Down,DownTimeout,controlSource);
							break;
						case Control_Bollard_Stop:
							BollardControlStop=ControlDisable;
							break;
					}
					break;
					
				case Remote:
					switch(remoteValue)
					{
						case RemoteUp:
							BollardControlUp=ControlDisable;
							//��������ʱ��¼
							Control_Event_Save(Control_Bollard_Up,UpTimeout,controlSource);
							break;
						case RemoteDown:
							BollardControlDown=ControlDisable;
							//���½���ʱ��¼
							Control_Event_Save(Control_Bollard_Down,DownTimeout,controlSource);
							break;
						case RemoteStop:
							BollardControlStop=ControlDisable;
							break;
						case RemoteEmergency:
							BollardControlUp=ControlDisable;
							Control_Event_Save(Control_Bollard_Up,UpTimeout,controlSource);
							break;
					}
					break;
				
				case Synchro:
					switch(cascadeConnectionAft)
					{
						case Connection0:
//									BollardControlStop=ControlDisable;
							break;
						case Connection1:
							BollardControlUp=ControlDisable;
							//��������ʱ��¼
							Control_Event_Save(Control_Bollard_Up,UpTimeout,controlSource);
							break;
						case Connection2:
							BollardControlDown=ControlDisable;
							//���½���ʱ��¼
							Control_Event_Save(Control_Bollard_Down,DownTimeout,controlSource);
							break;
						case Connection3:
							//δ֪���ϣ�������
							break;
					}
					break;
				case Bollard1://�»���������ʱ
					BollardControlUp=ControlDisable;
					Control_Event_Save(Control_Bollard_Up,UpTimeout,controlSource);
					break;
				case Bollard2:
					BollardControlUp=ControlDisable;
					Control_Event_Save(Control_Bollard_Up,UpTimeout,controlSource);							
					break;
				case Bollard3:
					BollardControlUp=ControlDisable;
					Control_Event_Save(Control_Bollard_Up,UpTimeout,controlSource);						
					break;
				case Host:
					
					break;
				case Unknown:
					break;
			}
			controlOn=0;
		}
	}
	TIM_Cmd(TIM2,DISABLE);
	TIM_SetCounter(TIM2,0);//
}
		
//ͨ�ö�ʱ��3�жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��3!
void TIM3_Int_Init(u16 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱ��ʹ��
	
	//��ʱ��TIM3��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	//TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�

	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���

	TIM_Cmd(TIM3,DISABLE);  //ʹ��TIMx
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //���TIMx�����жϱ�־ 
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�	
}

//��ʱ��3�жϷ������
void TIM3_IRQHandler(void)   //TIM3�ж�
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //���TIM3�����жϷ������
		{
			TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //���TIMx�����жϱ�־ 	
			//TimeOut=1;
			//TIM_Cmd(TIM3, DISABLE);  //ʹ��TIMx	
			timeSliceCnt++;
			if(!(timeSliceCnt%9))//90msʱ��Ƭ
			{
				if(bottonPressed)
				{
					bottonPressed=0;
					TIM_Cmd(TIM2,DISABLE); //����������ʱ����
					TIM_SetCounter(TIM2,0);
					//stop��������
					if(bollardControlType==Control_Bollard_Stop)
					{
						BollardControlStop=ControlEnable;
						BollardControlUp=ControlDisable;
						BollardControlDown=ControlDisable;
						controlOn=1;
						controlSource=Botton;//botton
						if(alarmOn==1)
						{
							AlarmControl=ControlDisable;
							alarmOn=0;
						}
						TIM_Cmd(TIM2,ENABLE);
					}
					//up��down��Ԥ���ٶ���
					else
					{
						bottonFlag=1;
						BollardControlStop=ControlDisable;
						BollardControlUp=ControlDisable;
						BollardControlDown=ControlDisable;
						if(controlOn==1)
							controlOn=0;
						AlarmControl=ControlEnable;
						alarmOn=1;
						maxSliceCnt=0;//��ʼ��ʱ
					}
				}
				if(remoteControl)
				{
					remoteControl=0;	
					TIM_Cmd(TIM2,DISABLE); //����������ʱ����
					TIM_SetCounter(TIM2,0);
					//stop��emergency��������
					if(remoteValue==RemoteStop)
					{
						BollardControlStop=ControlEnable;
						BollardControlUp=ControlDisable;
						BollardControlDown=ControlDisable;
						controlOn=1;
						controlSource=Remote;//remote
						if(alarmOn==1)
						{
							AlarmControl=ControlDisable;
							alarmOn=0;
						}
						TIM_Cmd(TIM2,ENABLE);
					}
					else if(remoteValue==RemoteEmergency)
					{
						BollardControlStop=ControlDisable;
						BollardControlDown=ControlDisable;
						BollardControlUp=ControlEnable;
						AlarmControl=ControlEnable;
						controlOn=1;
						controlSource=Remote;
						TIM_Cmd(TIM2,ENABLE); 
					}
					//up��down��Ԥ���ٶ���
					else
					{
						remoteFlag=1;
						BollardControlStop=ControlDisable;
						BollardControlUp=ControlDisable;
						BollardControlDown=ControlDisable;
						if(controlOn==1)
							controlOn=0;
						AlarmControl=ControlEnable;
						alarmOn=1;
						maxSliceCnt=0;//��ʼ��ʱ
					}
				}
				if(cascadeChange)
				{
					cascadeChange=0;
					//cascadeFlag=1;
					//AlarmControl=ControlEnable;
					//alarmOn=1;
					if(bottonFlag!=1)
					{
						TIM_Cmd(TIM2,DISABLE);
						TIM_SetCounter(TIM2,0);						
						switch(cascadeConnectionAft)
						{
							case Connection0://not exist
	// 							BollardControlStop=ControlEnable;
	// 							BollardControlUp=ControlDisable;
	// 							BollardControlDown=ControlDisable;
								break;
							case Connection1:
								BollardControlDown=ControlDisable;
								BollardControlUp=ControlEnable;
// 								if(controlOn==1)
// 									timeSliceCnt=1;
								controlOn=1;								
								controlSource=Synchro;//cascade
								//��SD����¼������������¼
								break;
							case Connection2:
								BollardControlUp=ControlDisable;
								BollardControlDown=ControlEnable;
// 								if(controlOn==1)
// 									timeSliceCnt=1;
								controlOn=1;								
								controlSource=Synchro;//cascade
								//��SD����¼�������½���¼
								break;
							case Connection3:
								//δ֪���ϣ�������
								break;
						}
						//cascadeFlag=0;
						//controlOn=1;								
						//controlSource=Synchro;//cascade
						TIM_Cmd(TIM2,ENABLE); //
					}
					//maxSliceCnt=0;//��ʼ��ʱ
				}
				if(statusChange)
				{
					statusChange=0;
					switch(bollardStatus)
					{
						case Emergency:
							AlarmControl=ControlEnable;//��������ͬʱԤ��
							alarmOn=1;
							StatusOutput1=1;
							StatusOutput2=0;
							break;
						case UpIng:
							StatusOutput1=1;
							StatusOutput2=0;
							break;
						case DownIng:
							StatusOutput1=0;
							StatusOutput2=1;							
							break;
						case NoAction:
							StatusOutput1=1;//CTL=1,out=0
							StatusOutput2=1;
							break;
					}
				}
			}
			if(!(timeSliceCnt%49))//490msʱ��Ƭ
			{
				if(limitReach)//&&controlOn)
				{
					limitReach=0;
// 					if(controlOn)
// 					{
// 						TIM_Cmd(TIM2,DISABLE); //
// 						TIM_SetCounter(TIM2,0);
						switch(limitValue)
						{
							case upperLimitReach:
								if(BollardControlUp==ControlEnable)//||bollardStatus==Emergency
								{
									TIM_Cmd(TIM2,DISABLE); //
									TIM_SetCounter(TIM2,0);
									BollardControlUp=ControlDisable;
									controlOn=0;
									//
									Control_Event_Save(Control_Bollard_Up,Top,controlSource);
								}
								else
									Error_Event_Save(illegalUp,controlSource);
								break;
							case lowerLimitReach:
								if(BollardControlDown==ControlEnable)
								{
									TIM_Cmd(TIM2,DISABLE); //
									TIM_SetCounter(TIM2,0);
									BollardControlDown=ControlDisable;
									controlOn=0;
									//
									Control_Event_Save(Control_Bollard_Down,Bottom,controlSource);
								}
								else
									Error_Event_Save(slideDown,controlSource);
								break;
							case 0://����
								break;
						}
// 					}
// 					else
// 					{
// 						switch(limitValue)
// 						{
// 							case upperLimitReach:
// 								BollardControlUp=ControlDisable;
// 								Error_Event_Save(illegalUp,Unknown);
// 								break;
// 							case lowerLimitReach:
// 								BollardControlDown=ControlDisable;
// 								Error_Event_Save(illegalDown,Unknown);
// 								break;
// 							case 0://����
// 								break;
// 						}
// 					}
					//controlOn=0;
				}
			}
			if(!(timeSliceCnt%100))//1sʱ��Ƭ
			{
				maxSliceCnt++;
				timeSliceCnt=0; ////��0��0�����κ���				
// 				if((!(maxSliceCnt%7))&&controlOn)
// 				{
// 					switch(controlSource)
// 					{
// 						case Botton:
// 							switch(bollardControlType)
// 							{
// 								case Control_Bollard_Up:
// 									BollardControlUp=ControlDisable;
// 									//��������ʱ��¼
// 									Control_Event_Save(Control_Bollard_Up,UpTimeout,controlSource);
// 									break;
// 								case Control_Bollard_Down:
// 									BollardControlDown=ControlDisable;
// 									//���½���ʱ��¼
// 									Control_Event_Save(Control_Bollard_Down,DownTimeout,controlSource);
// 									break;
// 								case Control_Bollard_Stop:
// 									BollardControlStop=ControlDisable;
// 									break;
// 							}
// 							break;
// 							
// 						case Remote:
// 							break;
// 						
// 						case Synchro:
// 							switch(cascadeConnection)
// 							{
// 								case Connection0:
// //									BollardControlStop=ControlDisable;
// 									break;
// 								case Connection1:
// 									BollardControlUp=ControlDisable;
// 									//��������ʱ��¼
// 									Control_Event_Save(Control_Bollard_Up,UpTimeout,controlSource);
// 									break;
// 								case Connection2:
// 									BollardControlDown=ControlDisable;
// 									//���½���ʱ��¼
// 									Control_Event_Save(Control_Bollard_Down,DownTimeout,controlSource);
// 									break;
// 								case Connection3:
// 									//δ֪���ϣ�������
// 									break;
// 							}
// 							break;
// 						case Bollard1://�»���������ʱ
// 							BollardControlUp=ControlDisable;
// 							Control_Event_Save(Control_Bollard_Up,UpTimeout,controlSource);
// 							break;
// 						case Bollard2:
// 							BollardControlUp=ControlDisable;
// 							Control_Event_Save(Control_Bollard_Up,UpTimeout,controlSource);							
// 							break;
// 						case Bollard3:
// 							BollardControlUp=ControlDisable;
// 							Control_Event_Save(Control_Bollard_Up,UpTimeout,controlSource);						
// 							break;
// 						case Host:
// 							
// 							break;
// 					}
// 					controlOn=0;
// 				}
				if(!(maxSliceCnt%30))
				{
					if(alarmOn)
					{
						AlarmControl=ControlDisable;
						alarmOn=0;
						if(bottonFlag)
						{
							switch(bollardControlType)
							{
								case Control_Bollard_Up:
									if(groundCoilStatus==NoCoilTrigger)
									{
										BollardControlUp=ControlEnable;
										controlOn=1;
										controlSource=Botton;//botton
										TIM_Cmd(TIM2,ENABLE); //
									}
									//��SD����¼����ť������¼
									//Control_Event_Save(Control_Bollard_Up,Top,Botton);
									break;
								case Control_Bollard_Down:
									BollardControlDown=ControlEnable;
									controlOn=1;
									controlSource=Botton;//botton
									TIM_Cmd(TIM2,ENABLE); //
									//��SD����¼����ť�½���¼
									break;
								case Control_Bollard_Stop://won't happen
									BollardControlStop=ControlEnable;
									BollardControlUp=ControlDisable;
									BollardControlDown=ControlDisable;
									break;
							}
							bottonFlag=0;
// 							controlOn=1;
// 							controlSource=Botton;//botton
// 							TIM_Cmd(TIM2,ENABLE); //
						}
						if(remoteFlag)
						{
							switch(remoteValue)
							{
								case RemoteUp:
									if(groundCoilStatus==NoCoilTrigger)
									{
										BollardControlUp=ControlEnable;
										controlOn=1;				
										controlSource=Remote;//remote
										TIM_Cmd(TIM2,ENABLE); //
									}
									//��SD����¼����ť������¼
									//Control_Event_Save(Control_Bollard_Up,Top,Botton);
									break;
								case RemoteDown:
									BollardControlDown=ControlEnable;
									controlOn=1;				
									controlSource=Remote;//remote
									TIM_Cmd(TIM2,ENABLE); //
									//��SD����¼����ť�½���¼
									break;
								case RemoteStop://won't happen
									BollardControlStop=ControlEnable;
									BollardControlUp=ControlDisable;
									BollardControlDown=ControlDisable;
									break;
								case RemoteEmergency://won't happen
									break;
							}
							remoteFlag=0;
// 							controlOn=1;				
// 							controlSource=Remote;//remote
// 							TIM_Cmd(TIM2,ENABLE); //
						}
// 						if(cascadeFlag)
// 						{
// 							switch(cascadeConnection)
// 							{
// 								case Connection0:
// 									BollardControlStop=ControlEnable;
// 									BollardControlUp=ControlDisable;
// 									BollardControlDown=ControlDisable;
// 									break;
// 								case Connection1:
// 									BollardControlUp=ControlEnable;
// 									//��SD����¼������������¼
// 									break;
// 								case Connection2:
// 									BollardControlDown=ControlEnable;
// 									//��SD����¼�������½���¼
// 									break;
// 								case Connection3:
// 									//δ֪���ϣ�������
// 									break;
// 							}
// 							cascadeFlag=0;
// 							controlOn=1;								
// 							controlSource=Synchro;//cascade
// 						}
					}
					maxSliceCnt=0;
				}
		}
	}
}

//porttimer.c
// void TIM4_Int_Init(u16 arr,u16 psc)
// {
//   TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
// 	NVIC_InitTypeDef NVIC_InitStructure;

// 	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //ʱ��ʹ��
// 	
// 	//��ʱ��TIM3��ʼ��
// 	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
// 	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
// 	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
// 	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
// 	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
//  
// 	//TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�

// 	//�ж����ȼ�NVIC����
// 	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM3�ж�
// 	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
// 	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  //�����ȼ�3��
// 	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
// 	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���
// 	
// 	TIM_Cmd(TIM4, DISABLE);  //ʹ��TIMx					 
// 	TIM_ClearITPendingBit(TIM4, TIM_IT_Update  );
// 	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�
// }

// void TIM4_IRQHandler(void)   //TIM4�ж�
// {
// 	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)  //���TIM3�����жϷ������
// 		{
// 			TIM_ClearITPendingBit(TIM4, TIM_IT_Update  );  //���TIMx�����жϱ�־ 	
// 			//time_2s=1;
// 			TIM_Cmd(TIM4, DISABLE);  //ʹ��TIMx	
// 		}
// }


//TIMER5 ʱ��Ƭ
void TIM5_Int_Init(u16 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE); //ʱ��ʹ��
	
	//��ʱ��TIM3��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�

	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���

	TIM_Cmd(TIM5, DISABLE);  //ʹ��TIMx
	//TIM_Cmd(TIM5, ENABLE);
	TIM_ClearITPendingBit(TIM5, TIM_IT_Update  );  //���TIMx�����жϱ�־ 
	TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�	

}

void TIM5_IRQHandler(void)   //TIM5ʱ��Ƭ�ж�
{
	if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)  //���TIM3�����жϷ������
		{
			TIM_ClearITPendingBit(TIM5, TIM_IT_Update);  //���TIMx�����жϱ�־ 	
			//
// 			timeSliceCnt++;
// 			if(!timeSliceCnt%9)//90msʱ��Ƭ
// 			{
// 				if(bottonPressed)
// 				{
// 					bottonPressed=0;
// 					bottonFlag=1;
// 					AlarmControl=ControlEnable;
// 					alarmOn=1;					
// 				}
// 				if(remoteControl)
// 				{
// 					remoteControl=0;
// 					remoteFlag=1;
// 					AlarmControl=ControlEnable;
// 					alarmOn=1;						
// 				}
// 				if(cascadeChange)
// 				{
// 					cascadeChange=0;
// 					cascadeFlag=1;
// 					AlarmControl=ControlEnable;
// 					alarmOn=1;
// 				}
// 				if(statusChange)
// 				{
// 					statusChange=0;
// 					switch(bollardStatus)
// 					{
// 						case Emergency:
// 							StatusOutput1=1;
// 							StatusOutput2=0;
// 							break;
// 						case UpIng:
// 							StatusOutput1=1;
// 							StatusOutput2=0;
// 							break;
// 						case DownIng:
// 							StatusOutput1=0;
// 							StatusOutput2=1;							
// 							break;
// 						case 0:
// 							StatusOutput1=0;
// 							StatusOutput2=0;
// 							break;
// 					}
// 				}
// 			}
// 			if(!timeSliceCnt%49)//490msʱ��Ƭ
// 			{
// 				if(limitReach&&controlOn)
// 				{
// 					limitReach=0;
// 					switch(limitValue)
// 					{
// 						case upperLimitReach:
// 							BollardControlUp=ControlDisable;
// 							//
// 							Control_Event_Save(Control_Bollard_Up,Top,controlSource);
// 							break;
// 						case lowerLimitReach:
// 							BollardControlDown=ControlDisable;
// 							//
// 							Control_Event_Save(Control_Bollard_Down,Bottom,controlSource);
// 							break;
// 					}
// 					controlOn=0;
// 				}
// 			}
// 			if(!timeSliceCnt%99)//990msʱ��Ƭ
// 			{
// 				maxSliceCnt++;
// 				timeSliceCnt=0;
// 				if(!maxSliceCnt%30)
// 				{
// 					if(alarmOn)
// 					{
// 						AlarmControl=ControlDisable;
// 						alarmOn=0;
// 						if(bottonFlag)
// 						{
// 							switch(bollardControlType)
// 							{
// 								case Control_Bollard_Up:
// 									BollardControlUp=ControlEnable;
// 									//��SD����¼����ť������¼
// 									//Control_Event_Save(Control_Bollard_Up,Top,Botton);
// 									break;
// 								case Control_Bollard_Down:
// 									BollardControlDown=ControlEnable;
// 									//��SD����¼����ť�½���¼
// 									break;
// 								case Control_Bollard_Stop:
// 									BollardControlStop=ControlEnable;
// 									BollardControlUp=ControlDisable;
// 									BollardControlDown=ControlDisable;
// 									break;
// 							}
// 							bottonFlag=0;
// 							controlOn=1;
// 							controlSource=0x00;//botton
// 						}
// 						if(remoteFlag)
// 						{
// 							remoteFlag=0;
// 							
// 							controlSource=0x01;//remote
// 						}
// 						if(cascadeFlag)
// 						{
// 							switch(cascadeConnection)
// 							{
// 								case Connection0:
// 									BollardControlStop=ControlEnable;
// 									BollardControlUp=ControlDisable;
// 									BollardControlDown=ControlDisable;
// 									break;
// 								case Connection1:
// 									BollardControlUp=ControlEnable;
// 									//��SD����¼������������¼
// 									break;
// 								case Connection2:
// 									BollardControlDown=ControlEnable;
// 									//��SD����¼�������½���¼
// 									break;
// 								case Connection3:
// 									//δ֪���ϣ�������
// 									break;
// 							}
// 							cascadeFlag=0;
// 							controlOn=1;								
// 							controlSource=0x02;//cascade
// 						}
// 					}
// 					maxSliceCnt=0;
// 				}
// 				if((!maxSliceCnt%7)&&controlOn)
// 				{
// 						switch(bollardControlType)
// 						{
// 							case Control_Bollard_Up:
// 								BollardControlUp=ControlDisable;
// 								//��������ʱ��¼
// 								Control_Event_Save(Control_Bollard_Up,UpTimeout,controlSource);
// 								break;
// 							case Control_Bollard_Down:
// 								BollardControlDown=ControlDisable;
// 								//���½���ʱ��¼
// 								Control_Event_Save(Control_Bollard_Down,DownTimeout,controlSource);
// 								break;
// 							case Control_Bollard_Stop:
// 								BollardControlStop=ControlDisable;
// 								break;
// 						}
// 						controlOn=0;
// 				}
//			}
			//TIM_Cmd(TIM5, DISABLE);  //ʹ��TIMx	
	  }
}

void TIM6_Int_Init(u16 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE); //ʱ��ʹ��
	
	//��ʱ��TIM3��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�

	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���
	TIM_Cmd(TIM6, DISABLE);  //ʹ��TIMx					 
}
//u8 BUF[8]={0XA0};
/*void TIM6_IRQHandler(void)   //TIM3�ж�
{
	if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)  //���TIM3�����жϷ������
		{
			TIM_ClearITPendingBit(TIM6, TIM_IT_Update  );  //���TIMx�����жϱ�־ 	
				Can_Send_Msg(BUF,8);//����8���ֽ�
			  inREADER_ST=0;		
			  Can_Send_Msg1(BUF,8);//����8���ֽ�
			  outREADER_ST=0;
			TIM_Cmd(TIM5, ENABLE);  //ʹ��TIMx	
		}
}*/


