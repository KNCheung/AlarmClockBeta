#include <rtthread.h>
#include <board.h>
#include <application.h>

#include <IIC.h>
#include <REG.h>

extern rt_mq_t key_mq;
extern rt_event_t reg_event;
extern uint32_t reg_output[REG_SCREEN_NUMBER]; 

void rt_thread_alarm_entry (void *parameter);
void fnSetAlarmClock(uint8_t hour,uint8_t minute,uint8_t second);
void fnAlarm(void);

void rt_thread_alarm_entry (void *parameter)
{
	uint8_t buf;
	uint16_t t;
	rt_uint32_t e;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE); 
	if (PWR_GetFlagStatus(PWR_FLAG_WU)==SET)
	{
		rt_kprintf("Waken up by DS3231\n");
		fnAlarm();
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
			fnSetAlarmClock(t/100,t%100,0);
	}
}

void fnSetAlarmClock(uint8_t h,uint8_t m,uint8_t s)
{
	uint8_t t;
	rt_kprintf("\n===Enter Alarm Clock Mode===\n");
	rt_event_send(reg_event,REG_ALARM_MSK);
	reg_output[REG_ALARM] = 0x80808080;
	
	/* Set Alarm Time */
	IIC_Write(DS3231_ADDRESS,DS3231_A1_S,(s%10)|((s/10)<<4));
	IIC_Write(DS3231_ADDRESS,DS3231_A1_M,(m%10)|((m/10)<<4));
	IIC_Write(DS3231_ADDRESS,DS3231_A1_H,(h%10)|((h/10)<<4));
	
	/* Clear Alarm Flag */
	t = IIC_Read(DS3231_ADDRESS,DS3231_CON_STA);
	t &= 0xFC;
	IIC_Write(DS3231_ADDRESS,DS3231_CON_STA,t);
	
	rt_thread_delay_hmsm(0,0,0,600);
	reg_output[REG_ALARM] = 0xFFFFFFFF;
	rt_thread_delay_hmsm(0,0,0,400);
	
	lp();
}

void fnAlarm(void)
{
	rt_uint32_t e;
	reg_output[REG_ALARM] = 0xF7F7F7F7;
	rt_event_send(reg_event,REG_ALARM_MSK);
	rt_thread_delay_hmsm(0,0,3,0);
	rt_event_recv(reg_event,REG_ALARM_MSK,RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR,0,&e);
	return;
}
