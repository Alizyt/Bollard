#ifndef _RECORDMANAGE_H
#define _RECORDMANAGE_H
#include "sys.h"
#include "controlfunction.h"//

void Power_Event_Save(ePowerEventType type);
void Control_Event_Save(eBollardControlType type,eBollardControlStatus status,eBollardControlSource source);
void Status_Event_Save(eBollardStatus status);
void Error_Event_Save(eErrorEventType type,eBollardControlSource source);
void Setting_Event_Save(eSettingEventType type,unsigned short int* regbuf,u8 regcount);
u8 Event_Read(eEventType type,u32 recordindx,unsigned short int* recordDataOuthex);
void SD_Write_Process(u8* pucFrame,u16 len,eEventType type);
void SD_Process(u8* pucFrame,u16 len,u8 type);
u8 asciitohex(u8* ascii_adr);
void hextoascii(u8* ascii_adr,u8 hexd );

#endif
