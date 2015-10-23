#ifndef __EXTI_H
#define __EXIT_H	 
#include "sys.h"
#include "rtc.h"//

u8 Slide_Time_Compare(_calendar_obj comp);
void EXTIX_Init(void);//外部中断初始化
//void EXTI0_IRQHandler(void);
//void EXTI1_IRQHandler(void);
void EXTI2_IRQHandler(void);
//void EXTI3_IRQHandler(void);
//void EXTI4_IRQHandler(void);
void EXTI9_5_IRQHandler(void);
//void EXTI15_10_IRQHandler(void);
#endif

