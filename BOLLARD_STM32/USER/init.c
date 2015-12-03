#include "init.h"
#include "led.h"
#include "delay.h"
#include "key.h"
#include "beep.h"
#include "rtc.h"
#include "timer.h"
#include "mb.h"
#include "exfuns.h"
#include "malloc.h"
#include "mmc_sd.h"
#include "controlfunction.h"
#include "recordmanage.h"
#include "exti.h"

u8 upDownTime=6;//最大升降时间，可设置
u8 controllerAddr;
u8 sdBug[20];//
void systemInit(void)
{
				delay_init();						//延时函数初始化
				NVIC_Configuration(); 	//设置NVIC中断分组2：2位抢占优先级，2位响应优先级
				LED_Init();							//LED端口初始化
				ControlFunction_Init();	//初始化与按键连接的硬件接口
				controllerAddr=ADDR1*8+ADDR2*4+ADDR3*2+ADDR4; //获取控制板地址
				//KEY_Init();							
				//BEEP_Init();
				EXTIX_Init();					//外部中断初始化
				RTC_Init();							//实时时钟初始化
				//TIM5_Int_Init(59999,7199); //定时器3初始化，定时6s  (59999+1)*(7199+1)/72M=6s
				TIM2_Int_Init(upDownTime*10000-1,7199);//updown time 6s
				TIM3_Int_Init(99,7199);//时间片定时器5初始化，定时10ms
				//MODBUS初始化 RTU模式 从机地址为controllerAddr USART2 9600 无校验
				eMBInit(MB_RTU,controllerAddr,0x02,9600,MB_PAR_NONE);
				delay_ms(200);//
				exfuns_init();					//为FATFS相关变量申请内存
				mem_init (SRAMIN);			//初始化内部内存池
				while(SD_Initialize())	//检测SD卡
				{
					delay_ms(200);
					LED0=!LED0;						//DS0闪烁
				}
 				LED0=0;
				f_mount(0,fs[0]);				//挂载SD卡
				
				write_file(sdBug,1,0,"zzz");//sd bug, first file damaged	
				Power_Event_Save(powerOn);
				//RTC_Set(2015,11,23,20,10,0);

}

