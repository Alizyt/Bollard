/* ----------------------- System includes ----------------------------------*/
#include "stdlib.h"
#include "string.h"
#include "controlfunction.h"

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbframe.h"
#include "mbproto.h"
#include "mbconfig.h"

/* ----------------------- Defines ------------------------------------------*/
#define MB_PDU_FUNC_CTRL_TYPE_OFF           ( MB_PDU_DATA_OFF )
#define MB_PDU_FUNC_CTRL_SIZE               ( 2 )

#define MB_CTRL_OK													( 0 )
#define MB_CTRL_ERR													( 1 )
/* ----------------------- Static functions ---------------------------------*/
eMBException    prveMBError2Exception( eMBErrorCode eErrorCode );

/* ----------------------- Start implementation -----------------------------*/
#if MB_FUNC_CTRL_ENABLED > 0

eMBException
eMBFuncControlBollard( UCHAR * pucFrame, USHORT * usLen )
{
    UCHAR          *pucFrameCur;

    eMBException    eStatus = MB_EX_NONE;
    //eMBErrorCode    eRegStatus;

    if( *usLen == ( MB_PDU_FUNC_CTRL_SIZE + MB_PDU_SIZE_MIN ) )
    {
				/* Set the current PDU data pointer to the beginning. */
				pucFrameCur = &pucFrame[MB_PDU_FUNC_OFF];
				*usLen = MB_PDU_FUNC_OFF;
				
				/* First byte contains the function code. */
				*pucFrameCur++ = MB_FUNC_CONTROL_BOLLARD;
				*usLen += 1;
				
				*pucFrameCur++ = pucFrame[MB_PDU_FUNC_CTRL_TYPE_OFF];
				*usLen += 1;
				
				if(Bollard_Control(pucFrame[MB_PDU_FUNC_CTRL_TYPE_OFF],Remote))
				{
					*pucFrameCur++ = MB_CTRL_OK;
				}
				else
				{
					*pucFrameCur++ = MB_CTRL_ERR;
					eStatus = MB_EX_SLAVE_DEVICE_FAILURE;
				}
				*usLen += 1;
    }
    else
    {
        /* Can't be a valid read input register request because the length
         * is incorrect. */
        eStatus = MB_EX_ILLEGAL_DATA_VALUE;
    }
    return eStatus;
}

#endif
