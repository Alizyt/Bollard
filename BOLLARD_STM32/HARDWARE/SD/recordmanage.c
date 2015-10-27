#include "recordmanage.h"
#include "rtc.h"
#include "mbproto.h"
#include "fattester.h"
#include "controlfunction.h"


void Power_Event_Save(ePowerEventType type)
{
	u8 PowerSaveBuf[64];
	PowerSaveBuf[0]=type;
	SD_Write_Process(PowerSaveBuf,1,powerEvent);
}
void Control_Event_Save(eBollardControlType type,eBollardControlStatus status,eBollardControlSource source)
{
	u8 ControlSaveBuf[64];
	ControlSaveBuf[0]=type;
	ControlSaveBuf[1]=status;
	ControlSaveBuf[2]=source;
	SD_Write_Process(ControlSaveBuf,3,controlEvent);
}
void Status_Event_Save(eBollardStatus status)
{
	u8 StatusSave;
	StatusSave=status;
	SD_Write_Process(&StatusSave,1,statusEvent);
}
void Error_Event_Save(eErrorEventType type,eBollardControlSource source)
{
	u8 ErrorSaveBuf[64];
	ErrorSaveBuf[0]=type;
	ErrorSaveBuf[1]=source;
	SD_Write_Process(ErrorSaveBuf,2,errorEvent);
}
void Setting_Event_Save(eSettingEventType type,unsigned short int* regbuf,u8 regcount)
{
	u8 SettingSaveBuf[64],i;
	SettingSaveBuf[0]=type;
	for(i=0;i<regcount;i++)
		SettingSaveBuf[i+1]=(*regbuf++);
	SD_Write_Process(SettingSaveBuf,regcount+1,settingEvent);
}
u8 Event_Read(eEventType type,u32 recordindx)
{
	u8 recordDataOut[32];
	switch(type)
	{
		case powerEvent:
			read_file(recordDataOut,recordindx,18,"POWER");
			break;
		case controlEvent:
			read_file(recordDataOut,recordindx,22,"BOLLARD");
			break;
		case errorEvent:
			read_file(recordDataOut,recordindx,20,"ERROR");
			break;
	}
	//...recordDataOut×ª´æ

	return 0;//
}

void SD_Write_Process(u8* pucFrame,u16 len,eEventType type)
{
	u8 i;
//	u8 sdFramehex[128];
	u8 sdFrameascii[256];
	u8* sdFrameasciiAdr=sdFrameascii;
	
// 	for(i=0;i<len;i++)
// 	{
// 		sdFramehex[i]=*(pucFrame+i);
// 	}
	for(i=0;i<len;i++)
	{
		hextoascii(sdFrameasciiAdr,*pucFrame++);
		sdFrameasciiAdr+=2;
	}
	switch(type)
	{
		case powerEvent:
			write_file(sdFrameascii,1,2*len,"POWER");
			break;
		case controlEvent:
			write_file(sdFrameascii,1,2*len,"BOLLARD");
			break;
		case errorEvent:
			write_file(sdFrameascii,1,2*len,"ERROR");
			break;
	}
}


//test
void SD_Process(u8* pucFrame,u16 len,u8 type)
{
	u8 i;
//	u8 sdFramehex[256];
	u8 sdFrameascii[512];
	u8* sdFrameasciiAdr=sdFrameascii;
	
// 	for(i=0;i<len;i++)
// 	{
// 		sdFramehex[i]=*(pucFrame+i);
// 	}
	switch(type)
	{
		case MB_FUNC_WRITE_MULTIPLE_REGISTERS:
		{
// 			sdFramehex[len]=calendar.w_year%256;
// 			sdFramehex[len+1]=calendar.w_year/256;
// 			sdFramehex[len+2]=calendar.w_month;
// 			sdFramehex[len+3]=calendar.w_date;
// 			sdFramehex[len+4]=calendar.hour;
// 			sdFramehex[len+5]=calendar.min;
// 			sdFramehex[len+6]=calendar.sec;
			for(i=0;i<len;i++)
			{
				hextoascii(sdFrameasciiAdr,*pucFrame++);
				sdFrameasciiAdr+=2;
			}
			write_file(sdFrameascii,1,2*len,"BOLLARD");
			break;
		}
	}
}

u8 asciitohex(u32* ascii_adr)
{
	u8 hexr;
	//if((USART_RX_BUF[ascii_adr]>='0')&&(USART_RX_BUF[ascii_adr]<='9')) 
	if(((*ascii_adr)>='0')&&((*ascii_adr)<='9')) 
	{	
		hexr=*(ascii_adr++)-'0';
		//hexr=USART_RX_BUF[ascii_adr++]-'0';
	}
	else
	{	
		hexr=*(ascii_adr++)-'A'+0xa;
		//hexr=USART_RX_BUF[ascii_adr++]-'A'+0xa;
	}
	if(((*ascii_adr)>='0')&&((*ascii_adr)<='9')) 
	//if((USART_RX_BUF[ascii_adr]>='0')&&(USART_RX_BUF[ascii_adr]<='9')) 
	{	
		hexr=hexr*16+(*ascii_adr)-'0';
		//hexr=hexr*16+USART_RX_BUF[ascii_adr]-'0';
	}
	else
	{	
		hexr=hexr*16+(*ascii_adr)-'A'+0xa;
		//hexr=hexr*16+USART_RX_BUF[ascii_adr]-'A'+0xa;
	}
	return(hexr);
}

void hextoascii(u8* ascii_adr,u8 hexd )
{
	u8 hexr;
	hexr=hexd/16;
	if((hexr>=0)&&(hexr<=9))
	{	
		*ascii_adr++=hexr+'0';
		//USART_TX_BUF[ascii_adr++]=hexr+'0';
	}
	else
	{	
		*ascii_adr++=hexr-0xa+'A';
		//USART_TX_BUF[ascii_adr++]=hexr-0xa+'A';
	}
	hexr=hexd%16;
	if((hexr>=0)&&(hexr<=9))
	{	
		*ascii_adr=hexr+'0';
		//USART_TX_BUF[ascii_adr]=hexr+'0';
	}
	else
	{	
		*ascii_adr=hexr-0xa+'A';
		//USART_TX_BUF[ascii_adr]=hexr-0xa+'A';
	}
}
