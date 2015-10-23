#ifndef _CONTROLFUNCTION_H
#define _CONTROLFUNCTION_H

#include "sys.h"

typedef enum
{
	Control_Bollard_Up=0x01,
	Control_Bollard_Down=0x02,
	Control_Bollard_Stop=0x03,
	//Control_Bollard_Emergency //本地手动、外同步、远程
	//Control_Alarm_Pre,
	//Control_Alarm_Ing
}eBollardControlType;

typedef enum
{
	Botton=0x00,
	Remote=0x01,
	Synchro=0x02,
	Host=0x03,
	Bollard1=0x04,
	Bollard2=0x05,
	Bollard3=0x06,
	Unknown=0x07
}eBollardControlSource;

typedef enum
{
	BollardNum1=0x0002,
	BollardNum2=0x0004,
	BollardNum3=0x0008,
	BollardNumAll=0x000E
}eBollardNum;

typedef enum
{
	Top=0x00,
	Bottom=0x01,
	UpTimeout=0x02,
	DownTimeout=0x03
}eBollardControlStatus;

typedef enum
{
	BottonUp=0x01,
	BottonDown=0x02,
	BottonStop=0x03
}eBollardBottonValue;

typedef enum
{
	RemoteUp=0x01,
	RemoteDown=0x02,
	RemoteStop=0x03,
	RemoteEmergency=0x04
}eBollardRemoteValue;

typedef enum
{
	noLimitReach=0x00,
	upperLimitReach=0x01,
	lowerLimitReach=0x02
}eLimitValue;

typedef enum
{
	Connection0=0x00,
	Connection1=0x02,
	Connection2=0x01,
	Connection3=0x03
}eCascadeConnection;

typedef enum
{
	NoAction=0x00,
	Emergency=0x01,
	UpIng	=0x02,
	DownIng=0x03
}eBollardStatus;

typedef enum
{
	NoCoilTrigger=0x00,
	CoilFrontTrigger=0x01,
	CoilRearTrigger=0x02
}eGroundCoilStatus;

typedef enum
{
	powerEvent,
	controlEvent,
	errorEvent,
	settingEvent
}eEventType;

typedef enum
{
	powerOn=0x00,
	powerOff=0x01
}ePowerEventType;

typedef enum
{
	slideDown=0x00,
	illegalUp=0x01,
	illegalDown=0x02
	//...
}eErrorEventType;

typedef enum
{
	upDownTimeSet=0x00,
	slideTimeSet=0x01,
	groundCoilSet=0x02,
	synchroSet=0x03,
	remoteSet=0x04
}eSettingEventType;

// u8 bottonPressed=0;//bottonValue=0,bollardControlType;
// eBollardBottonValue bottonValue;
// eBollardControlType bollardControlType;
// u8 remoteControl;
// u8 limitReach=0;//limitValue=0,limitValuePre=0,
// eLimitValue limitValue,limitValuePre;
// u8 statusChange=0;//bollardStatus=0,bollardStatusPre=0,
// eBollardStatus bollardStatus,bollardStatusPre;
// u8 cascadeChange=0;//cascadeConnection=0,cascadeConnectionPre=0,
// eCascadeConnection cascadeConnection,cascadeConnectionPre;

// u8 alarmOn,controlOn;
// eBollardControlSource controlSource;
// u32 timeSliceCnt=0,maxSliceCnt=0;

#define ControlEnable				0x00
#define ControlDisable			0x01
#define BottonPressed				0x01
#define BottonReleased			0x00
#define ReachLimit					0x01
#define CascadeConnected		0x01
#define StatusTrue					0x00	//IN7 IN8 IN11逻辑反
#define CoilTrue						0x01
//#define ActionTrue					0x01	//改版后删除，IN7 IN8逻辑反

// #define BottonUp						0x01
// #define BottonDown					0x02
// #define BottonStop					0x03

// #define LimitUp1						0x01
// #define LimitUp2						0x02
// #define LimitUp3						0x03
// #define LimitLow1					0x04
// #define LimitLow2					0x05
// #define LimitLow3					0x06

// #define upperLimitReach			0x01
// #define lowerLimitReach			0x02

// #define Connection0					0x00
// #define Connection1					0x02
// #define Connection2					0x01
// #define Connection3					0x03

// #define Emergency						0x01
// #define UpIng								0x02
// #define DownIng							0x03
/******Program Control Output Definition (CPU output)******/
#define BollardControlUp 	 	PEout(12)
#define BollardControlDown 	PEout(11)
#define BollardControlStop 	PDout(9)
#define AlarmControl				PDout(10)
//#define AlarmControlPre 		PAout(2)
//#define AlarmControlIng			PAout(2)
#define StatusOutput1				PEout(3)
#define StatusOutput2				PEout(4)
/******Botton Control && Statuas Input Definition (CPU input)******/
#define BollardBottonUp 		PBin(2)
#define BollardBottonDown 	PEin(7)
#define BollardBottonStop 	PEin(8)

#define UpperLimit1 				PCin(5)
#define UpperLimit2 				PDin(8)
#define UpperLimit3 				PEin(15)
#define LowerLimit1 				PCin(4)
#define LowerLimit2 				PAin(7)
#define LowerLimit3 				PBin(0)

#define CascadeConnection1	PEin(9)
#define CascadeConnection2	PEin(10)

#define EmergencyUp					PEin(14)
#define BollardUpIng				PEin(13)
#define BollardDownIng			PBin(1)

#define GroundCoilFront			PAin(5)
#define GroundCoilRear			PAin(4)

#define MXOF								PEin(2)

void ControlFunction_Init(void);
u8 Bollard_Control(eBollardControlType type,eBollardControlSource source);
eBollardBottonValue Botton_Scan(u8 mode);
void Botton_Control(void);
//void Alarm_Control(eBollardControlType type);
void Alarm_Control(void);
eLimitValue Limit_Scan(void);
eCascadeConnection Cascade_Connection_Scan(void);
eBollardStatus Bollard_Status_Scan(void);
eGroundCoilStatus Ground_Coil_Scan(void);

#endif

