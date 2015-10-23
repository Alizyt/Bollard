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

//通用定时器2中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
void TIM2_Int_Init(u16 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //时钟使能
	
	//定时器TIM3初始化
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
 
	//TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //使能指定的TIM3中断,允许更新中断

	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器

	TIM_Cmd(TIM2,DISABLE);  //使能TIMx
	TIM_SetCounter(TIM2,0);//
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update  );  //清除TIMx更新中断标志 
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE ); //使能指定的TIM3中断,允许更新中断	
}

void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  //检查TIM2更新中断发生与否
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  //清除TIMx更新中断标志 	
		
		if(controlOn)
		{
			switch(controlSource)
			{
				case Botton:
					switch(bollardControlType)
					{
						case Control_Bollard_Up:
							BollardControlUp=ControlDisable;
							//存上升超时记录
							Control_Event_Save(Control_Bollard_Up,UpTimeout,controlSource);
							break;
						case Control_Bollard_Down:
							BollardControlDown=ControlDisable;
							//存下降超时记录
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
							//存上升超时记录
							Control_Event_Save(Control_Bollard_Up,UpTimeout,controlSource);
							break;
						case RemoteDown:
							BollardControlDown=ControlDisable;
							//存下降超时记录
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
							//存上升超时记录
							Control_Event_Save(Control_Bollard_Up,UpTimeout,controlSource);
							break;
						case Connection2:
							BollardControlDown=ControlDisable;
							//存下降超时记录
							Control_Event_Save(Control_Bollard_Down,DownTimeout,controlSource);
							break;
						case Connection3:
							//未知故障，不动作
							break;
					}
					break;
				case Bollard1://下滑后上升超时
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
		
//通用定时器3中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器3!
void TIM3_Int_Init(u16 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能
	
	//定时器TIM3初始化
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
 
	//TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //使能指定的TIM3中断,允许更新中断

	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器

	TIM_Cmd(TIM3,DISABLE);  //使能TIMx
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //清除TIMx更新中断标志 
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //使能指定的TIM3中断,允许更新中断	
}

//定时器3中断服务程序
void TIM3_IRQHandler(void)   //TIM3中断
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //检查TIM3更新中断发生与否
		{
			TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //清除TIMx更新中断标志 	
			//TimeOut=1;
			//TIM_Cmd(TIM3, DISABLE);  //使能TIMx	
			timeSliceCnt++;
			if(!(timeSliceCnt%9))//90ms时间片
			{
				if(bottonPressed)
				{
					bottonPressed=0;
					TIM_Cmd(TIM2,DISABLE); //否则连按计时不对
					TIM_SetCounter(TIM2,0);
					//stop立即动作
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
					//up、down先预警再动作
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
						maxSliceCnt=0;//开始计时
					}
				}
				if(remoteControl)
				{
					remoteControl=0;	
					TIM_Cmd(TIM2,DISABLE); //否则连按计时不对
					TIM_SetCounter(TIM2,0);
					//stop、emergency立即动作
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
					//up、down先预警再动作
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
						maxSliceCnt=0;//开始计时
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
								//存SD卡记录，级联上升记录
								break;
							case Connection2:
								BollardControlUp=ControlDisable;
								BollardControlDown=ControlEnable;
// 								if(controlOn==1)
// 									timeSliceCnt=1;
								controlOn=1;								
								controlSource=Synchro;//cascade
								//存SD卡记录，级联下降记录
								break;
							case Connection3:
								//未知故障，不动作
								break;
						}
						//cascadeFlag=0;
						//controlOn=1;								
						//controlSource=Synchro;//cascade
						TIM_Cmd(TIM2,ENABLE); //
					}
					//maxSliceCnt=0;//开始计时
				}
				if(statusChange)
				{
					statusChange=0;
					switch(bollardStatus)
					{
						case Emergency:
							AlarmControl=ControlEnable;//紧急上升同时预警
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
			if(!(timeSliceCnt%49))//490ms时间片
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
							case 0://抖动
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
// 							case 0://抖动
// 								break;
// 						}
// 					}
					//controlOn=0;
				}
			}
			if(!(timeSliceCnt%100))//1s时间片
			{
				maxSliceCnt++;
				timeSliceCnt=0; ////改0，0整除任何数				
// 				if((!(maxSliceCnt%7))&&controlOn)
// 				{
// 					switch(controlSource)
// 					{
// 						case Botton:
// 							switch(bollardControlType)
// 							{
// 								case Control_Bollard_Up:
// 									BollardControlUp=ControlDisable;
// 									//存上升超时记录
// 									Control_Event_Save(Control_Bollard_Up,UpTimeout,controlSource);
// 									break;
// 								case Control_Bollard_Down:
// 									BollardControlDown=ControlDisable;
// 									//存下降超时记录
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
// 									//存上升超时记录
// 									Control_Event_Save(Control_Bollard_Up,UpTimeout,controlSource);
// 									break;
// 								case Connection2:
// 									BollardControlDown=ControlDisable;
// 									//存下降超时记录
// 									Control_Event_Save(Control_Bollard_Down,DownTimeout,controlSource);
// 									break;
// 								case Connection3:
// 									//未知故障，不动作
// 									break;
// 							}
// 							break;
// 						case Bollard1://下滑后上升超时
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
									//存SD卡记录，按钮上升记录
									//Control_Event_Save(Control_Bollard_Up,Top,Botton);
									break;
								case Control_Bollard_Down:
									BollardControlDown=ControlEnable;
									controlOn=1;
									controlSource=Botton;//botton
									TIM_Cmd(TIM2,ENABLE); //
									//存SD卡记录，按钮下降记录
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
									//存SD卡记录，按钮上升记录
									//Control_Event_Save(Control_Bollard_Up,Top,Botton);
									break;
								case RemoteDown:
									BollardControlDown=ControlEnable;
									controlOn=1;				
									controlSource=Remote;//remote
									TIM_Cmd(TIM2,ENABLE); //
									//存SD卡记录，按钮下降记录
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
// 									//存SD卡记录，级联上升记录
// 									break;
// 								case Connection2:
// 									BollardControlDown=ControlEnable;
// 									//存SD卡记录，级联下降记录
// 									break;
// 								case Connection3:
// 									//未知故障，不动作
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

// 	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //时钟使能
// 	
// 	//定时器TIM3初始化
// 	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
// 	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
// 	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
// 	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
// 	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
//  
// 	//TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE ); //使能指定的TIM3中断,允许更新中断

// 	//中断优先级NVIC设置
// 	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM3中断
// 	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
// 	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  //从优先级3级
// 	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
// 	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器
// 	
// 	TIM_Cmd(TIM4, DISABLE);  //使能TIMx					 
// 	TIM_ClearITPendingBit(TIM4, TIM_IT_Update  );
// 	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE ); //使能指定的TIM3中断,允许更新中断
// }

// void TIM4_IRQHandler(void)   //TIM4中断
// {
// 	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)  //检查TIM3更新中断发生与否
// 		{
// 			TIM_ClearITPendingBit(TIM4, TIM_IT_Update  );  //清除TIMx更新中断标志 	
// 			//time_2s=1;
// 			TIM_Cmd(TIM4, DISABLE);  //使能TIMx	
// 		}
// }


//TIMER5 时间片
void TIM5_Int_Init(u16 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE); //时钟使能
	
	//定时器TIM3初始化
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE ); //使能指定的TIM3中断,允许更新中断

	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器

	TIM_Cmd(TIM5, DISABLE);  //使能TIMx
	//TIM_Cmd(TIM5, ENABLE);
	TIM_ClearITPendingBit(TIM5, TIM_IT_Update  );  //清除TIMx更新中断标志 
	TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE ); //使能指定的TIM3中断,允许更新中断	

}

void TIM5_IRQHandler(void)   //TIM5时间片中断
{
	if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)  //检查TIM3更新中断发生与否
		{
			TIM_ClearITPendingBit(TIM5, TIM_IT_Update);  //清除TIMx更新中断标志 	
			//
// 			timeSliceCnt++;
// 			if(!timeSliceCnt%9)//90ms时间片
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
// 			if(!timeSliceCnt%49)//490ms时间片
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
// 			if(!timeSliceCnt%99)//990ms时间片
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
// 									//存SD卡记录，按钮上升记录
// 									//Control_Event_Save(Control_Bollard_Up,Top,Botton);
// 									break;
// 								case Control_Bollard_Down:
// 									BollardControlDown=ControlEnable;
// 									//存SD卡记录，按钮下降记录
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
// 									//存SD卡记录，级联上升记录
// 									break;
// 								case Connection2:
// 									BollardControlDown=ControlEnable;
// 									//存SD卡记录，级联下降记录
// 									break;
// 								case Connection3:
// 									//未知故障，不动作
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
// 								//存上升超时记录
// 								Control_Event_Save(Control_Bollard_Up,UpTimeout,controlSource);
// 								break;
// 							case Control_Bollard_Down:
// 								BollardControlDown=ControlDisable;
// 								//存下降超时记录
// 								Control_Event_Save(Control_Bollard_Down,DownTimeout,controlSource);
// 								break;
// 							case Control_Bollard_Stop:
// 								BollardControlStop=ControlDisable;
// 								break;
// 						}
// 						controlOn=0;
// 				}
//			}
			//TIM_Cmd(TIM5, DISABLE);  //使能TIMx	
	  }
}

void TIM6_Int_Init(u16 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE); //时钟使能
	
	//定时器TIM3初始化
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE ); //使能指定的TIM3中断,允许更新中断

	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器
	TIM_Cmd(TIM6, DISABLE);  //使能TIMx					 
}
//u8 BUF[8]={0XA0};
/*void TIM6_IRQHandler(void)   //TIM3中断
{
	if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)  //检查TIM3更新中断发生与否
		{
			TIM_ClearITPendingBit(TIM6, TIM_IT_Update  );  //清除TIMx更新中断标志 	
				Can_Send_Msg(BUF,8);//发送8个字节
			  inREADER_ST=0;		
			  Can_Send_Msg1(BUF,8);//发送8个字节
			  outREADER_ST=0;
			TIM_Cmd(TIM5, ENABLE);  //使能TIMx	
		}
}*/


