#include "port.h"

#include "rs485.h"
#include "stm32f10x.h"
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- static functions ---------------------------------*/
static void prvvUARTTxReadyISR( void );
static void prvvUARTRxISR( void );

/* ----------------------- Start implementation -----------------------------*/
/**
* @brief ���ƽ��պͷ���״̬
* @param xRxEnable ����ʹ�ܡ�
* xTxEnable ����ʹ��
* @retval None
*/
void
vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
    /* If xRXEnable enable serial receive interrupts. If xTxENable enable
     * transmitter empty interrupts.
     */
	if(xRxEnable)
	{
		//ʹ�ܽ��պͽ����ж�
		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
		//MAX485���� �͵�ƽΪ����ģʽ
		RS485_TX_EN=0;
		//GPIO_ResetBits(GPIOD,GPIO_Pin_8);
	}
	else
	{
		USART_ITConfig(USART2, USART_IT_RXNE, DISABLE); 
		//MAX485���� �ߵ�ƽΪ����ģʽ
		RS485_TX_EN=1;
		//GPIO_SetBits(GPIOD,GPIO_Pin_8);
	}

	if(xTxEnable)
	{
		//ʹ�ܷ�������ж�
		USART_ITConfig(USART2, USART_IT_TC, ENABLE);
	}
	else
	{
		//��ֹ��������ж�
		USART_ITConfig(USART2, USART_IT_TC, DISABLE);
	}
}

/**
* @brief ���ڳ�ʼ��
* @param ucPORT ���ں�
* ulBaudRate ������
* ucDataBits ����λ
* eParity У��λ 
* @retval None
*/
BOOL
xMBPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
// 	GPIO_InitTypeDef GPIO_InitStructure;
// 	USART_InitTypeDef USART_InitStructure;
// 	NVIC_InitTypeDef NVIC_InitStructure;
	
	(void)ucPORT; //���޸Ĵ���
	(void)ucDataBits; //���޸�����λ����
	(void)eParity; //���޸�У���ʽ
	
	RS485_Init(ulBaudRate);

// 	//ʹ��USART1��GPIOA
// 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | 
// 	RCC_APB2Periph_USART1, ENABLE);

// 	//GPIOA9 USART1_Tx
// 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
// 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
// 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //�������
// 	GPIO_Init(GPIOA, &GPIO_InitStructure);
// 	//GPIOA.10 USART1_Rx
// 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
// 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
// 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //��������
// 	GPIO_Init(GPIOA, &GPIO_InitStructure);

// 	USART_InitStructure.USART_BaudRate = ulBaudRate; //ֻ�޸Ĳ�����
// 	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
// 	USART_InitStructure.USART_StopBits = USART_StopBits_1;
// 	USART_InitStructure.USART_Parity = USART_Parity_No;
// 	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
// 	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
// 	//���ڳ�ʼ��
// 	USART_Init(USART1, &USART_InitStructure);
// 	//ʹ��USART1
// 	USART_Cmd(USART1, ENABLE);

// 	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
// 	//�趨USART1 �ж����ȼ�
// 	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
// 	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
// 	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
// 	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
// 	NVIC_Init(&NVIC_InitStructure);

// 	//�������485���ͺͽ���ģʽ
// 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
// 	//GPIOD.8
// 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; 
// 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
// 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
// 	GPIO_Init(GPIOD, &GPIO_InitStructure); 

	return TRUE;
}

BOOL
xMBPortSerialPutByte( CHAR ucByte )
{
    /* Put a byte in the UARTs transmit buffer. This function is called
     * by the protocol stack if pxMBFrameCBTransmitterEmpty( ) has been
     * called. */
	//��������
	USART_SendData(USART2, ucByte);
	return TRUE;
}

BOOL
xMBPortSerialGetByte( CHAR * pucByte )
{
    /* Return the byte in the UARTs receive buffer. This function is called
     * by the protocol stack after pxMBFrameCBByteReceived( ) has been called.
     */
	//��������
	*pucByte = USART_ReceiveData(USART2);
	return TRUE;
}

/* Create an interrupt handler for the transmit buffer empty interrupt
 * (or an equivalent) for your target processor. This function should then
 * call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
 * a new character can be sent. The protocol stack will then call 
 * xMBPortSerialPutByte( ) to send the character.
 */
static void prvvUARTTxReadyISR( void )
{
	//mb.c eMBInit������
	//pxMBFrameCBTransmitterEmpty = xMBRTUTransmitFSM 
	//����״̬��
   pxMBFrameCBTransmitterEmpty(  );
}

/* Create an interrupt handler for the receive interrupt for your target
 * processor. This function should then call pxMBFrameCBByteReceived( ). The
 * protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
 * character.
 */
static void prvvUARTRxISR( void )
{
	//mb.c eMBInit������
	//pxMBFrameCBByteReceived = xMBRTUReceiveFSM
	//����״̬��
  pxMBFrameCBByteReceived(  );
}


void USART2_IRQHandler(void)
{
	//���������ж�
	if(USART_GetITStatus(USART2, USART_IT_RXNE) == SET)
	{
		prvvUARTRxISR(); 
		//����жϱ�־λ 
		USART_ClearITPendingBit(USART2, USART_IT_RXNE); 
	}

	//��������ж�
	if(USART_GetITStatus(USART2, USART_IT_TC) == SET)
	{
		prvvUARTTxReadyISR();
		//����жϱ�־
		USART_ClearITPendingBit(USART2, USART_IT_TC);
	}
}
