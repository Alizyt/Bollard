#include "mb.h" 
#include "controlfunction.h"
// #include "..\..\include\adc.h" 
// #include "..\..\include\lm75.h" 
// #include "..\..\include\timer.h" 
// #include "..\..\include\simulate.h" 
// #include "..\..\include\encode.h" 
// #include "..\..\include\hardware.h"
// #include "..\..\include\systick.h"
#include "timer.h"
#include "recordmanage.h"
#include "rtc.h"

 unsigned short int  usRegInputBuf[10]={0x0000,0xfe02,0x1203,0x1304,0x1405,0x1506,0x1607,0x1708,0x1809};
 //unsigned short int *usRegHoldingBuf=usRegInputBuf;        //һ�������õ� �Ĵ������� ��ַ0-7
 unsigned short int  usRegHoldingBuf[64];
 
 unsigned char REG_INPUT_START=0x00,REG_HOLDING_START=0x00;//�Ĵ�����ʼ��ַ
 unsigned char REG_INPUT_NREGS=12,REG_HOLDING_NREGS=64;//�Ĵ�������
 unsigned char usRegInputStart=0,usRegHoldingStart=0;//
 
 unsigned char REG_COILS_START=0x20,REG_DISCRETE_START=0x30;
 unsigned char REG_COILS_SIZE=8,REG_DISCRETE_SIZE=8;
 unsigned short int ucRegCoilsBuf[10],ucRegDiscreteBuf[10];
 
 /***********************add******************************/
 u8 remoteControl;
 eBollardRemoteValue remoteValue;
 extern u8 slideTime;
 extern u8 upDownTime;
 extern u8 groundCoilOnOff,synchroOnOff,remoteOnOff;

 /***********************add*****************************/
 
/**
* @brief ����Ĵ�����������������Ĵ����ɶ���������д��
* @param pucRegBuffer ��������ָ��
* usAddress �Ĵ�����ʼ��ַ
* usNRegs �Ĵ�������
* @retval eStatus �Ĵ���״̬
*/
eMBErrorCode 
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
	eMBErrorCode eStatus = MB_ENOERR;
	int16_t iRegIndex;

	//��ѯ�Ƿ��ڼĴ�����Χ��
	//Ϊ�˱��⾯�棬�޸�Ϊ�з�������
	if( ( (int16_t)usAddress >= REG_INPUT_START ) \
	&& ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
	{
		//��ò���ƫ���������β�����ʼ��ַ-����Ĵ����ĳ�ʼ��ַ
		iRegIndex = ( int16_t )( usAddress - REG_INPUT_START );
		//�����ֵ
		while( usNRegs > 0 )
		{
			//��ֵ���ֽ�
			*pucRegBuffer++ = ( uint8_t )( usRegInputBuf[iRegIndex] >> 8 );
			//��ֵ���ֽ�
			*pucRegBuffer++ = ( uint8_t )( usRegInputBuf[iRegIndex] & 0xFF );
			//ƫ��������
			iRegIndex++;
			//�������Ĵ��������ݼ�
			usNRegs--;
		}
	}
	else
	{
	//���ش���״̬���޼Ĵ��� 
	eStatus = MB_ENOREG;
	}

	return eStatus;
}

/**
* @brief ���ּĴ����������������ּĴ����ɶ����ɶ���д
* @param pucRegBuffer ������ʱ--��������ָ�룬д����ʱ--��������ָ��
* usAddress �Ĵ�����ʼ��ַ
* usNRegs �Ĵ�������
* eMode ������ʽ��������д
* @retval eStatus �Ĵ���״̬
*/
eMBErrorCode 
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode )
{
	//����״̬
	eMBErrorCode eStatus = MB_ENOERR;
	//ƫ����
	int16_t iRegIndex, iRegIndexCp;

	//�жϼĴ����ǲ����ڷ�Χ��
	if( ( (int16_t)usAddress >= REG_HOLDING_START ) \
	&& ( usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS ) )
	{
		//����ƫ����
		iRegIndex = ( int16_t )( usAddress - REG_HOLDING_START );
		iRegIndexCp = iRegIndex;
		switch ( eMode )
		{
			//����������(read record and status) 
			//step1: read data from sdcard
			//step2: copy the matched data to usRegHoldingBuf[iRegIndex]
			case MB_REG_READ:
//******************************add*******************************************//
			switch(iRegIndex)
			{
				case 13:
					Event_Read(powerEvent,1,&usRegHoldingBuf[13]);
					break;
				case 22:
					Event_Read(controlEvent,1,&usRegHoldingBuf[22]);
					break;
				case 33:
					Event_Read(statusEvent,1,&usRegHoldingBuf[33]);
					break;
				case 42:
					Event_Read(errorEvent,1,&usRegHoldingBuf[42]);
					break;
			}
//******************************add*******************************************//
			while( usNRegs > 0 )
			{
				*pucRegBuffer++ = ( uint8_t )( usRegHoldingBuf[iRegIndex] >> 8 );
				*pucRegBuffer++ = ( uint8_t )( usRegHoldingBuf[iRegIndex] & 0xFF );
				iRegIndex++;
				usNRegs--;
			}
			break;

			//д�������� 
			case MB_REG_WRITE:
			while( usNRegs > 0 )
			{
				usRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
				usRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
				iRegIndex++;
				usNRegs--;
			}
//******************************add*******************************************//
			switch (iRegIndexCp)
			{	
				//notice!!! usRegHoldingBuf[]=>unsigned short int, need transform???test!!!
				case 1://control
					if(remoteOnOff==1)
					{
						if(usRegHoldingBuf[1]==0x00A0)
						{
							//Control_Bollard_Up
							remoteControl=1;
							remoteValue=RemoteUp;
						}
						else if(usRegHoldingBuf[1]==0x00B0)
						{
							//Control_Bollard_Down
							remoteControl=1;
							remoteValue=RemoteDown;
						}
						else if(usRegHoldingBuf[1]==0x00C0)
						{
							//Control_Bollard_Stop
							remoteControl=1;
							remoteValue=RemoteStop;
						}
						else if(usRegHoldingBuf[1]==0x00D0)
						{
							//Emergency
							remoteControl=1;
							remoteValue=RemoteEmergency;
						}
					}	
					break;
				
				case 2://set max up/down time
					upDownTime=usRegHoldingBuf[2];
					TIM2_Int_Init(upDownTime*10000-1,7199);
					//�����ü�¼
					Setting_Event_Save(upDownTimeSet,&usRegHoldingBuf[iRegIndexCp],1);
					break;
				case 3://set slidedown time
					slideTime=usRegHoldingBuf[3];
					//�����ü�¼
					Setting_Event_Save(slideTimeSet,&usRegHoldingBuf[iRegIndexCp],1);
					break;
				case 4://set groundCoilOnOff
					groundCoilOnOff=usRegHoldingBuf[4];
					//�����ü�¼
					Setting_Event_Save(groundCoilSet,&usRegHoldingBuf[iRegIndexCp],1);
					break;
				case 5://set synchroOnoff
					synchroOnOff=usRegHoldingBuf[5];
					//�����ü�¼
					Setting_Event_Save(synchroSet,&usRegHoldingBuf[iRegIndexCp],1);
					break;
				case 6://set remoteOnOff
					remoteOnOff=usRegHoldingBuf[6];
					//�����ü�¼
					Setting_Event_Save(remoteSet,&usRegHoldingBuf[iRegIndexCp],1);
					break;
				case 7://set time, 6 regs
					RTC_Set(usRegHoldingBuf[7],usRegHoldingBuf[8],usRegHoldingBuf[9],usRegHoldingBuf[10],usRegHoldingBuf[11],usRegHoldingBuf[12]);
					//�����ü�¼
					Setting_Event_Save(timeSet,&usRegHoldingBuf[iRegIndexCp],6);
					break;
				case 13:
					//...
					break;
			}
//******************************add*******************************************//
			break;
		}
	}
	else
	{
	//���ش���״̬
	eStatus = MB_ENOREG;
	}

	return eStatus;
}


/**
* @brief ��Ȧ�Ĵ���������������Ȧ�Ĵ����ɶ����ɶ���д
* @param pucRegBuffer ������---��������ָ�룬д����--��������ָ��
* usAddress �Ĵ�����ʼ��ַ
* usNRegs �Ĵ�������
* eMode ������ʽ��������д
* @retval eStatus �Ĵ���״̬
*/
eMBErrorCode
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode )
{
	//����״̬
	eMBErrorCode eStatus = MB_ENOERR;
	//�Ĵ�������
	int16_t iNCoils = ( int16_t )usNCoils;
	//�Ĵ���ƫ����
	int16_t usBitOffset;

	//���Ĵ����Ƿ���ָ����Χ��
	if( ( (int16_t)usAddress >= REG_COILS_START ) &&
	( usAddress + usNCoils <= REG_COILS_START + REG_COILS_SIZE ) )
	{
		//����Ĵ���ƫ����
		usBitOffset = ( int16_t )( usAddress - REG_COILS_START );
		switch ( eMode )
		{
			//������
			case MB_REG_READ:
			while( iNCoils > 0 )
			{
				*pucRegBuffer++ = xMBUtilGetBits( ucRegCoilsBuf, usBitOffset,
				( uint8_t )( iNCoils > 8 ? 8 : iNCoils ) );
				iNCoils -= 8;
				usBitOffset += 8;
			}
			break;

			//д����
			case MB_REG_WRITE:
			while( iNCoils > 0 )
			{
				xMBUtilSetBits( ucRegCoilsBuf, usBitOffset,
				( uint8_t )( iNCoils > 8 ? 8 : iNCoils ),
				*pucRegBuffer++ );
				iNCoils -= 8;
			}
			break;
		}

	}
	else
	{
		eStatus = MB_ENOREG;
	}
	return eStatus;
}

/**
* @brief ��������Ĵ���������������������Ĵ������ɶ�
* @param pucRegBuffer ������---��������ָ�룬д����--��������ָ��
* usAddress �Ĵ�����ʼ��ַ
* usNRegs �Ĵ�������
* eMode ������ʽ��������д
* @retval eStatus �Ĵ���״̬
*/
eMBErrorCode
eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
	//����״̬
	eMBErrorCode eStatus = MB_ENOERR;
	//�����Ĵ�������
	int16_t iNDiscrete = ( int16_t )usNDiscrete;
	//ƫ����
	uint16_t usBitOffset;

	//�жϼĴ���ʱ�����ƶ���Χ��
	if( ( (int16_t)usAddress >= REG_DISCRETE_START ) &&
	( usAddress + usNDiscrete <= REG_DISCRETE_START + REG_DISCRETE_SIZE ) )
	{
		//���ƫ����
		usBitOffset = ( uint16_t )( usAddress - REG_DISCRETE_START );

		while( iNDiscrete > 0 )
		{
			*pucRegBuffer++ = xMBUtilGetBits( ucRegDiscreteBuf, usBitOffset,
			( uint8_t)( iNDiscrete > 8 ? 8 : iNDiscrete ) );
			iNDiscrete -= 8;
			usBitOffset += 8;
		}

	}
	else
	{
		eStatus = MB_ENOREG;
	}
	return eStatus;
}
