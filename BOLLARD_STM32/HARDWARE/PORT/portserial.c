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
* @brief 控制接收和发送状态
* @param xRxEnable 接收使能、
* xTxEnable 发送使能
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
		//使能接收和接收中断
		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
		//MAX485操作 低电平为接收模式
		RS485_TX_EN=1;
		//GPIO_ResetBits(GPIOD,GPIO_Pin_8);
	}
	else
	{
		USART_ITConfig(USART2, USART_IT_RXNE, DISABLE); 
		//MAX485操作 高电平为发送模式
		RS485_TX_EN=0;
		//GPIO_SetBits(GPIOD,GPIO_Pin_8);
	}

	if(xTxEnable)
	{
		//使能发送完成中断
		USART_ITConfig(USART2, USART_IT_TC, ENABLE);
	}
	else
	{
		//禁止发送完成中断
		USART_ITConfig(USART2, USART_IT_TC, DISABLE);
	}
}

/**
* @brief 串口初始化
* @param ucPORT 串口号
* ulBaudRate 波特率
* ucDataBits 数据位
* eParity 校验位 
* @retval None
*/
BOOL
xMBPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
// 	GPIO_InitTypeDef GPIO_InitStructure;
// 	USART_InitTypeDef USART_InitStructure;
// 	NVIC_InitTypeDef NVIC_InitStructure;
	
	(void)ucPORT; //不修改串口
	(void)ucDataBits; //不修改数据位长度
	(void)eParity; //不修改校验格式
	
	RS485_Init(ulBaudRate);

// 	//使能USART1，GPIOA
// 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | 
// 	RCC_APB2Periph_USART1, ENABLE);

// 	//GPIOA9 USART1_Tx
// 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
// 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
// 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //推挽输出
// 	GPIO_Init(GPIOA, &GPIO_InitStructure);
// 	//GPIOA.10 USART1_Rx
// 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
// 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
// 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮动输入
// 	GPIO_Init(GPIOA, &GPIO_InitStructure);

// 	USART_InitStructure.USART_BaudRate = ulBaudRate; //只修改波特率
// 	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
// 	USART_InitStructure.USART_StopBits = USART_StopBits_1;
// 	USART_InitStructure.USART_Parity = USART_Parity_No;
// 	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
// 	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
// 	//串口初始化
// 	USART_Init(USART1, &USART_InitStructure);
// 	//使能USART1
// 	USART_Cmd(USART1, ENABLE);

// 	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
// 	//设定USART1 中断优先级
// 	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
// 	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
// 	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
// 	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
// 	NVIC_Init(&NVIC_InitStructure);

// 	//最后配置485发送和接收模式
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
	//发送数据
	USART_SendData(USART2, ucByte);
	return TRUE;
}

BOOL
xMBPortSerialGetByte( CHAR * pucByte )
{
    /* Return the byte in the UARTs receive buffer. This function is called
     * by the protocol stack after pxMBFrameCBByteReceived( ) has been called.
     */
	//接收数据
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
	//mb.c eMBInit函数中
	//pxMBFrameCBTransmitterEmpty = xMBRTUTransmitFSM 
	//发送状态机
   pxMBFrameCBTransmitterEmpty(  );
}

/* Create an interrupt handler for the receive interrupt for your target
 * processor. This function should then call pxMBFrameCBByteReceived( ). The
 * protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
 * character.
 */
static void prvvUARTRxISR( void )
{
	//mb.c eMBInit函数中
	//pxMBFrameCBByteReceived = xMBRTUReceiveFSM
	//接收状态机
  pxMBFrameCBByteReceived(  );
}


void USART2_IRQHandler(void)
{
	//发生接收中断
	if(USART_GetITStatus(USART2, USART_IT_RXNE) == SET)
	{
		prvvUARTRxISR(); 
		//清除中断标志位 
		USART_ClearITPendingBit(USART2, USART_IT_RXNE); 
	}

	//发送完成中断
	if(USART_GetITStatus(USART2, USART_IT_TC) == SET)
	{
		prvvUARTTxReadyISR();
		//清除中断标志
		USART_ClearITPendingBit(USART2, USART_IT_TC);
	}
}
