#include "controlfunction.h"
#include "delay.h"

extern u8 TimeOut;
extern u8 remoteEmergencyStatus;

/******Control I/O Port Initialization******/
void ControlFunction_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB
		|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE, ENABLE);	 //使能PA.B.C.D.E端口时钟
	//ControlInit、AlarmInit		
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_12;//端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOE, &GPIO_InitStructure);					 //根据设定参数初始化GPIOE.11.12
	GPIO_SetBits(GPIOE,GPIO_Pin_11|GPIO_Pin_12);	  //PE.11.12输出高
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;//端口配置
	GPIO_Init(GPIOD, &GPIO_InitStructure);					 //根据设定参数初始化GPIOD.9.10
	GPIO_SetBits(GPIOD,GPIO_Pin_9|GPIO_Pin_10);	  //PD.9.10输出高
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4;//端口配置
	GPIO_Init(GPIOE, &GPIO_InitStructure);					 //根据设定参数初始化GPIOE.3.4
	GPIO_SetBits(GPIOE,GPIO_Pin_3|GPIO_Pin_4);	  //PE.3.4输出高
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;//端口配置
	GPIO_Init(GPIOA, &GPIO_InitStructure);//根据设定参数初始化GPIOA6
	GPIO_ResetBits(GPIOA,GPIO_Pin_6);//PA6输出低
	//BottonInit
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2;//PB2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
 	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB2
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_7|GPIO_Pin_8;//PE7.8
	GPIO_Init(GPIOE, &GPIO_InitStructure);//初始化GPIOE7.8
	//
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_7|GPIO_Pin_4|GPIO_Pin_5;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4|GPIO_Pin_5;
	GPIO_Init(GPIOC, &GPIO_InitStructure);	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8|GPIO_Pin_11;
	GPIO_Init(GPIOD, &GPIO_InitStructure);	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_Init(GPIOE, &GPIO_InitStructure);	
}

/******Program Control Bollard Action******/
u8 Bollard_Control(eBollardControlType type,eBollardControlSource source)
{
	u8 result=1;
	switch (type)
	{
		case Control_Bollard_Up:
// 			Alarm_Control(Control_Alarm_Pre);
// 			BollardControlUp=ControlEnable;
// 			Alarm_Control(Control_Alarm_Ing);
// 			BollardControlUp=ControlDisable;
			BollardControlUp=ControlEnable;
 			if(BollardUpIng==StatusTrue)
 			{
 				//置上升中标志位，LED亮？
 			}
			StatusOutput1=1;
			StatusOutput2=0;
			TIM_Cmd(TIM3, ENABLE);
			while((UpperLimit1!=ReachLimit)||(UpperLimit2!=ReachLimit)||(UpperLimit3!=ReachLimit))
			{
				if(TimeOut!=1)
				{
					;
				}
				else
				{
					//记录超时事件
					result=0;
					break;
				}
			}
			TIM_Cmd(TIM3,DISABLE);
			TimeOut=0;
			BollardControlUp=ControlDisable;
			if(BollardUpIng!=StatusTrue)	
			{
				//复位上升中标志位，led灭？
			}
 			//存SD卡记录
			break;
		case Control_Bollard_Down:
// 			Alarm_Control(Control_Alarm_Pre);
// 			BollardControlDown=ControlEnable;
// 			Alarm_Control(Control_Alarm_Ing);
// 			BollardControlDown=ControlDisable;
			BollardControlDown=ControlEnable;
 			if(BollardDownIng==StatusTrue)
 			{
				//置下降中标志位，LED亮？
 			}
			TIM_Cmd(TIM3, ENABLE);
			while((LowerLimit1!=ReachLimit)||(LowerLimit2!=ReachLimit)||(LowerLimit3!=ReachLimit))
			{
				if(TimeOut!=1)
				{
					;
				}
				else
				{
					//记录超时事件
					result=0;
					break;
				}
			}
			TIM_Cmd(TIM3,DISABLE);
			TimeOut=0;
			BollardControlDown=ControlDisable;
			if(BollardDownIng!=StatusTrue)	
			{
				//复位下降中标志位，led灭？
			}
			StatusOutput1=0;
			StatusOutput2=1;
 			//存SD卡记录
			break;
		case Control_Bollard_Stop:
			BollardControlStop=ControlEnable;
			BollardControlUp=ControlDisable;
			BollardControlDown=ControlDisable;
			delay_ms(200);
			BollardControlStop=ControlDisable;
			StatusOutput1=0;
			StatusOutput2=0;
			break;
	}
	return result;
}

/******Scan Botton Pressed or not******/
eBollardBottonValue Botton_Scan(u8 mode)
{	 
	static u8 key_up=1;//按键按松开标志
	if(mode) 
		key_up=1;  //支持连按	
	if(key_up && (BollardBottonUp==BottonPressed
			|| BollardBottonDown==BottonPressed || BollardBottonStop==BottonPressed))
	{
		delay_ms(10);//去抖动 
		key_up=0;
		if(BollardBottonUp==BottonPressed)			 return BottonUp;
		else if(BollardBottonDown==BottonPressed)return BottonDown;
		else if(BollardBottonStop==BottonPressed)return BottonStop;
	}
	else if(BollardBottonUp==BottonReleased
						&& BollardBottonDown==BottonReleased && BollardBottonStop==BottonReleased)
		key_up=1; 	    
 	return 0;//无按键按下
}

/******Botton Control Process******/
void Botton_Control(void)
{
	u8 BottonValue;
	BottonValue=Botton_Scan(1);//
	if(BottonValue)
	{
		switch(BottonValue)
		{
			case BottonUp:
				Bollard_Control(Control_Bollard_Up,Botton);
				break;
			case BottonDown:
				Bollard_Control(Control_Bollard_Down,Botton);
				break;
			case BottonStop:
				Bollard_Control(Control_Bollard_Stop,Botton);
				break;
		}
	}
	else
		delay_ms(10);
}

/******Program Control Alarm******/
// void Alarm_Control(eBollardControlType type)
// {
// 	if(type==Control_Alarm_Pre)
// 	{
// 		AlarmControlPre=ControlEnable;
// 		delay_ms(2000);
// 		AlarmControlPre=ControlDisable;
// 	}
// 	else if(type==Control_Alarm_Ing)
// 	{
// 		AlarmControlIng=ControlEnable;
// 		delay_ms(5000);
// 		AlarmControlIng=ControlDisable;
// 	}
// }

/******Program Control Alarm******/
void Alarm_Control(void)
{
	AlarmControl=ControlEnable;
	delay_ms(5000);
	AlarmControl=ControlDisable;
}

/******Scan Upper or Lower Limit Reached or not******/
eLimitValue Limit_Scan(void)
{	 
// 		if(UpperLimit1==ReachLimit)			 	return LimitUp1;
// 		else if(UpperLimit2==ReachLimit)	return LimitUp2;
// 		else if(UpperLimit3==ReachLimit)	return LimitUp3;
// 		else if(LowerLimit1==ReachLimit)	return LimitLow1;
// 		else if(LowerLimit2==ReachLimit)	return LimitLow2;
// 		else if(LowerLimit3==ReachLimit)	return LimitLow3;	
	if((UpperLimit1==ReachLimit)&&(UpperLimit2==ReachLimit)&&(UpperLimit3==ReachLimit))
		return upperLimitReach;
	if((LowerLimit1==ReachLimit)&&(LowerLimit2==ReachLimit)&&(LowerLimit3==ReachLimit))
		return lowerLimitReach;	
 	return noLimitReach;//limit not reach
}

/******Scan Cascade Connected or not******/
eCascadeConnection Cascade_Connection_Scan(void)
{
	if((CascadeConnection1!=CascadeConnected)&&(CascadeConnection2!=CascadeConnected))
		return Connection0;
	else if((CascadeConnection1==CascadeConnected)&&(CascadeConnection2!=CascadeConnected))
		return Connection1;
	else if((CascadeConnection1!=CascadeConnected)&&(CascadeConnection2==CascadeConnected))
		return Connection2;
	else 
		return Connection3;
}

extern eLimitValue limitValue;
/******Scan Bollard Status******/
eBollardStatus Bollard_Status_Scan(void)
{
	if((EmergencyUp==StatusTrue||remoteEmergencyStatus)) 	return Emergency;
	else if(BollardUpIng==StatusTrue)		return UpIng;
	else if(BollardDownIng==StatusTrue)	return DownIng;
	else if(limitValue==upperLimitReach) return TopReached;
	else if(limitValue==lowerLimitReach) return BottomReached;
	
	return NoAction;
}

eGroundCoilStatus Ground_Coil_Scan(void)
{
	//if(GroundCoilFront==CoilTrue) return CoilFrontTrigger;
	//else if(GroundCoilRear==CoilTrue) return CoilRearTrigger;
	if(GroundCoil==CoilTrue) return CoilTrigger;
	
	return NoCoilTrigger;
}

