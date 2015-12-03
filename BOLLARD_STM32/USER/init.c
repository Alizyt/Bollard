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

u8 upDownTime=6;//�������ʱ�䣬������
u8 controllerAddr;
u8 sdBug[20];//
void systemInit(void)
{
				delay_init();						//��ʱ������ʼ��
				NVIC_Configuration(); 	//����NVIC�жϷ���2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
				LED_Init();							//LED�˿ڳ�ʼ��
				ControlFunction_Init();	//��ʼ���밴�����ӵ�Ӳ���ӿ�
				controllerAddr=ADDR1*8+ADDR2*4+ADDR3*2+ADDR4; //��ȡ���ư��ַ
				//KEY_Init();							
				//BEEP_Init();
				EXTIX_Init();					//�ⲿ�жϳ�ʼ��
				RTC_Init();							//ʵʱʱ�ӳ�ʼ��
				//TIM5_Int_Init(59999,7199); //��ʱ��3��ʼ������ʱ6s  (59999+1)*(7199+1)/72M=6s
				TIM2_Int_Init(upDownTime*10000-1,7199);//updown time 6s
				TIM3_Int_Init(99,7199);//ʱ��Ƭ��ʱ��5��ʼ������ʱ10ms
				//MODBUS��ʼ�� RTUģʽ �ӻ���ַΪcontrollerAddr USART2 9600 ��У��
				eMBInit(MB_RTU,controllerAddr,0x02,9600,MB_PAR_NONE);
				delay_ms(200);//
				exfuns_init();					//ΪFATFS��ر��������ڴ�
				mem_init (SRAMIN);			//��ʼ���ڲ��ڴ��
				while(SD_Initialize())	//���SD��
				{
					delay_ms(200);
					LED0=!LED0;						//DS0��˸
				}
 				LED0=0;
				f_mount(0,fs[0]);				//����SD��
				
				write_file(sdBug,1,0,"zzz");//sd bug, first file damaged	
				Power_Event_Save(powerOn);
				//RTC_Set(2015,11,23,20,10,0);

}

