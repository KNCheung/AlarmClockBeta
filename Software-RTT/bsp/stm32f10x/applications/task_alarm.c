#include <rtthread.h>
#include <board.h>
#include <application.h>

#include <IIC.h>
#include <REG.h>
#include <task_alarm.h>

#define ALARMER_PORT GPIOA
#define ALARMER_PIN  GPIO_Pin_8
#define ALARMER_CLOCK RCC_APB2Periph_GPIOA

extern rt_mq_t key_mq;
extern rt_event_t reg_event,en_event;
extern uint32_t reg_output[REG_SCREEN_NUMBER]; 
extern uint8_t clock_h,clock_m,clock_s;
extern uint8_t Setting[SETTING_LENGTH];

void rt_thread_alarm_entry (void *parameter);
ErrorStatus fnSetAlarmClock(uint8_t hour,uint8_t minute,uint8_t second);
void fnAlarm(void);
void AlarmerInit(void);

void rt_thread_alarm_entry (void *parameter)
{
	uint8_t buf;
	uint16_t t;
	rt_uint32_t e;
	
	AlarmerInit();
	SetAlarm(0);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE); 
	if (PWR_GetFlagStatus(PWR_FLAG_WU)==SET)
	{
		rt_kprintf("\n\nWaken up by DS3231\n");
		if ((IIC_Read(DS3231_ADDRESS, DS3231_CON_STA)) & 0x01) fnAlarm();
		IIC_Write(DS3231_ADDRESS, DS3231_CON_STA,0x80);
	}
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, DISABLE);

	while (rt_mq_recv(key_mq,&buf,sizeof(uint8_t),0)==RT_EOK);
	while (1)
	{
		do
		{
			rt_mq_recv(key_mq,&buf,sizeof(uint8_t),RT_WAITING_FOREVER);
		}while(buf!=0);
		t = 0;
		reg_output[REG_ALARM]=0xF777F7F7;
		rt_event_send(reg_event,REG_ALARM_MSK);
		
		rt_mq_recv(key_mq,&buf,sizeof(uint8_t),RT_WAITING_FOREVER);
		if (buf>9)
		{
			rt_event_recv(reg_event,REG_ALARM_MSK,RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR,0,&e);
			continue;
		}
		t = buf;
		reg_output[REG_ALARM]=(0x01000000*REG_HexToReg(buf)) | (0x00FFFFFF & reg_output[REG_ALARM]) | 0x80000000;

		rt_mq_recv(key_mq,&buf,sizeof(uint8_t),RT_WAITING_FOREVER);	
		if (buf>9)
		{
			rt_event_recv(reg_event,REG_ALARM_MSK,RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR,0,&e);
			continue;
		}
		t = t*10+buf;
		reg_output[REG_ALARM]=(0x00010000*REG_HexToReg(buf)) | (0xFF00FFFF & reg_output[REG_ALARM]);

		rt_mq_recv(key_mq,&buf,sizeof(uint8_t),RT_WAITING_FOREVER);		
		if (buf>9)
		{
			rt_event_recv(reg_event,REG_ALARM_MSK,RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR,0,&e);
			continue;
		}
		t = t*10+buf;
		reg_output[REG_ALARM]=(0x00000100*REG_HexToReg(buf)) | (0xFFFF00FF & reg_output[REG_ALARM]);

		rt_mq_recv(key_mq,&buf,sizeof(uint8_t),RT_WAITING_FOREVER);		
		if (buf>9)
		{
			rt_event_recv(reg_event,REG_ALARM_MSK,RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR,0,&e);
			continue;
		}
		t = t*10+buf;
		reg_output[REG_ALARM]=(0x00000001*REG_HexToReg(buf)) | (0xFFFFFF00 & reg_output[REG_ALARM]);

		rt_mq_recv(key_mq,&buf,sizeof(uint8_t),RT_WAITING_FOREVER);		
		if (buf<=9)
		{
			rt_event_recv(reg_event,REG_ALARM_MSK,RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR,0,&e);
			continue;
		}else
		{
			if (fnSetAlarmClock(t/100,t%100,buf==0x0B)==ERROR)
				rt_event_recv(reg_event,REG_ALARM_MSK,RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR,0,&e);

		}
	}
}

ErrorStatus fnSetAlarmClock(uint8_t h,uint8_t m,uint8_t setting)
{
	uint8_t t;
	
	if ((h>=24)||(m>=60))
		return ERROR;
	rt_kprintf("\n===Enter Alarm Clock Mode===\n");
	
	/* Set Alarm Time */
	t = IIC_Read(DS3231_ADDRESS,DS3231_CONTROL) & 0xFC;
	if (setting)
	{
		IIC_Write(DS3231_ADDRESS,DS3231_CONTROL,t | 0x01);
		IIC_Write(DS3231_ADDRESS,DS3231_A1_S,0x00);
		IIC_Write(DS3231_ADDRESS,DS3231_A1_M,(m%10)|((m/10)<<4));
		IIC_Write(DS3231_ADDRESS,DS3231_A1_H,(h%10)|((h/10)<<4));
	}else{
		IIC_Write(DS3231_ADDRESS,DS3231_CONTROL,t | 0x02);
		IIC_Write(DS3231_ADDRESS,DS3231_A2_M,(m%10)|((m/10)<<4));
		IIC_Write(DS3231_ADDRESS,DS3231_A2_H,(h%10)|((h/10)<<4));
		IIC_Write(DS3231_ADDRESS,DS3231_A2_D,0x80);
	}
	
	/* Clear Alarm Flag */
	t = IIC_Read(DS3231_ADDRESS,DS3231_CON_STA);
	t &= 0xFC;
	IIC_Write(DS3231_ADDRESS,DS3231_CON_STA,t);
	
	rt_event_send(en_event,EVENT_TEMP_CLOCK);
	rt_thread_delay_hmsm(0,0,1,0);
	
	rt_event_send(reg_event,REG_ALARM_MSK);
	reg_output[REG_ALARM] = REG_Convert(REG_HexToReg(m%10),REG_HexToReg(m/10),REG_HexToReg(h%10),REG_HexToReg(h/10),1,0);
	rt_thread_delay_hmsm(0,0,1,0);
	
	reg_output[REG_ALARM] = 0xFFFFFFFF;
	rt_thread_delay_hmsm(0,0,0,62);
	reg_output[REG_ALARM] = 0xFFFFFFFE;
	rt_thread_delay_hmsm(0,0,0,62);
	reg_output[REG_ALARM] = 0xFFFFFEBF;
	rt_thread_delay_hmsm(0,0,0,62);
	reg_output[REG_ALARM] = 0xFFFEBFF7;
	rt_thread_delay_hmsm(0,0,0,62);
	reg_output[REG_ALARM] = 0xFEBFF7FF;
	rt_thread_delay_hmsm(0,0,0,62);
	reg_output[REG_ALARM] = 0xBFF7FFFF;
	rt_thread_delay_hmsm(0,0,0,62);
	reg_output[REG_ALARM] = 0xF7FFFFFF;
	rt_thread_delay_hmsm(0,0,0,62);
	reg_output[REG_ALARM] = 0xFFFFFFFF;
	rt_thread_delay_hmsm(0,0,0,62);
	
	StandBy();
	return SUCCESS;
}

void AlarmerInit(void)
{
	GPIO_InitTypeDef usrGPIO;
	TIM_TimeBaseInitTypeDef usrTIMBase;
	TIM_OCInitTypeDef usrTIMOC;
	
	RCC_APB2PeriphClockCmd(ALARMER_CLOCK,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
	
	usrGPIO.GPIO_Mode = GPIO_Mode_AF_PP;
	usrGPIO.GPIO_Pin = ALARMER_PIN;
	usrGPIO.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ALARMER_PORT,&usrGPIO);
	
	usrTIMBase.TIM_ClockDivision = TIM_CKD_DIV1;
	usrTIMBase.TIM_CounterMode = TIM_CounterMode_Up;
	usrTIMBase.TIM_Period = 1023;
	usrTIMBase.TIM_Prescaler = 0;
	usrTIMBase.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1,&usrTIMBase);
	
	usrTIMOC.TIM_OCMode = TIM_OCMode_PWM1;
	usrTIMOC.TIM_OCIdleState = TIM_OCIdleState_Reset;
	usrTIMOC.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
	usrTIMOC.TIM_OCPolarity = TIM_OCPolarity_High;
	usrTIMOC.TIM_OCNPolarity = TIM_OCNPolarity_High;
	usrTIMOC.TIM_OutputState = TIM_OutputState_Enable;
	usrTIMOC.TIM_OutputNState = TIM_OutputNState_Disable;
	usrTIMOC.TIM_Pulse = 0;
	TIM_OC1Init(TIM1,&usrTIMOC);
	TIM_OC1PreloadConfig(TIM1,TIM_OCPreload_Enable);
	TIM_SetCompare1(TIM1,512);
}

void Animate(void)
{
	reg_output[REG_ALARM] = 0xFFFFFFFF;
	rt_thread_delay_hmsm(0,0,0,62);
	reg_output[REG_ALARM] = 0xF7FFFFFF;
	rt_thread_delay_hmsm(0,0,0,62);
	reg_output[REG_ALARM] = 0xBFF7FFFF;
	rt_thread_delay_hmsm(0,0,0,62);
	reg_output[REG_ALARM] = 0xFEBFF7FF;
	rt_thread_delay_hmsm(0,0,0,62);
	reg_output[REG_ALARM] = 0xFFFEBFF7;
	rt_thread_delay_hmsm(0,0,0,62);
	reg_output[REG_ALARM] = 0xFFFFFEBF;
	rt_thread_delay_hmsm(0,0,0,62);
	reg_output[REG_ALARM] = 0xFFFFFFFE;
	rt_thread_delay_hmsm(0,0,0,62);
	reg_output[REG_ALARM] = 0xFFFFFFFF;
	rt_thread_delay_hmsm(0,0,0,62);
}

void fnAlarm(void)
{
	rt_uint32_t e;
	uint8_t buf,int_m,t=0;
	uint16_t delay1,delay2,strength;
	rt_thread_delay_hmsm(0,0,3,0);
	int_m = (clock_m+5)%60;
	rt_kprintf("It is %d:%d now\nAlarm Level %d\n",clock_h,clock_m,t);
	rt_event_send(reg_event,REG_ALARM_MSK);
	while (rt_mq_recv(key_mq,&buf,sizeof(uint8_t),0)!=RT_EOK)
	{
		if (clock_m==int_m)
		{
			t+=1;
			int_m = (int_m+5)%60;
			rt_kprintf("Alarm Level %d\n",t);
		}
		switch (t)
		{
			case 0: delay1 = 500; delay2 = 4500; strength = 300; break;
			case 1: delay1 = 1000; delay2 = 2000; strength = 600; break;
			case 2: delay1 = 500; delay2 = 500; strength = 800; break;
			default: delay1 = 500; delay2 = 250; strength = 1000; break;
		}
		SetAlarm(strength);
		Animate();
		rt_thread_delay_hmsm(0,0,0,delay1);
		SetAlarm(0);
		rt_thread_delay_hmsm(0,0,0,delay2);
	}
	rt_event_recv(reg_event,REG_ALARM_MSK,RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR,0,&e);
	return;
}
