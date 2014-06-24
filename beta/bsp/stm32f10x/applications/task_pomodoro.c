#include <board.h>
#include <rtthread.h>
#include <application.h>
#include <stdlib.h>

rt_uint8_t pomodoro_stack[256];
struct rt_thread pomodoro_thread;

void task_pomodoro_init(void);
void rt_thread_pomodoro_entry(void* parameter);

void task_pomodoro_init(void)
{
	rt_err_t result;
	result = rt_thread_init(&pomodoro_thread,
	                        "Pomodoro",
	                        rt_thread_pomodoro_entry,
	                        RT_NULL,
	                        (rt_uint8_t*)pomodoro_stack,
	                        sizeof(pomodoro_stack),
	                        PRIO_POMODORO,
	                        2);
	if (result == RT_EOK) rt_thread_startup(&pomodoro_thread);
}


void rt_thread_pomodoro_entry(void* parameter)
{
	extern rt_event_t f_en,f_key,f_led;
	extern rt_mutex_t m_display;
	extern uint8_t ui_disp[4];
	uint32_t ts;
	rt_uint32_t e;
	uint8_t state,h,m,s;
	while (1)
	{
		rt_event_recv(f_en,F_EN_POMODORO,RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR ,0,&e);
		rt_event_recv(f_en,F_EN_POMODORO,RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR ,RT_WAITING_FOREVER,&e);
		if (rt_mutex_take(m_display,0)==RT_EOK)
		{
			rt_event_send(f_led,LED_RED);
			while (1)
			{
				ts=sRTC_GetTS()+25*60;
				ui_disp[0]=2;
				ui_disp[1]=5;
				ui_disp[2]=0x0F;
				ui_disp[3]=0x0F;
				rt_event_recv(f_key,F_KEY_31|F_KEY_33,RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR,0,&e);
				while (rt_event_recv(f_key,F_KEY_33,RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR,RT_TICK_PER_SECOND/10,&e)!=RT_EOK)
				{
					if (ts<sRTC_GetTS()) break;
					sRTC_GetRTC(ts-sRTC_GetTS(),NULL,NULL,NULL,NULL,&m,&s);
					m+=(s>0)?1:0;
					ui_disp[0]=m/10;
					ui_disp[1]=m%10;
					ui_disp[2]=0x0F;
					ui_disp[3]=0x0F;
				}
				if (ts>=sRTC_GetTS()) break;
				rt_event_send(f_led,LED_GREEN);
				
				state=1;
				rt_event_recv(f_key,F_KEY_31|F_KEY_33,RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR,0,&e);
				while (rt_event_recv(f_key,F_KEY_31|F_KEY_33,RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR,RT_TICK_PER_SECOND*1.5f,&e)!=RT_EOK)
				{
					state=!state;
					rt_event_send(f_led,state?LED_GREEN:LED_BLACK);
				}
				if (e&F_KEY_33) break;
				rt_event_send(f_led,LED_GREEN);
				
				ts=sRTC_GetTS()+5*60;
				ui_disp[0]=0x0F;
				ui_disp[1]=0x0F;
				ui_disp[2]=0x0F;
				ui_disp[3]=5;
				rt_event_recv(f_key,F_KEY_31|F_KEY_33,RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR,0,&e);
				while (rt_event_recv(f_key,F_KEY_33,RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR,RT_TICK_PER_SECOND/10,&e)!=RT_EOK)
				{
					if (ts<sRTC_GetTS()) break;
					sRTC_GetRTC(ts-sRTC_GetTS(),NULL,NULL,NULL,NULL,&m,&s);
					m+=(s>0)?1:0;
					ui_disp[0]=0x0F;
					ui_disp[1]=0x0F;
					ui_disp[2]=m/10;
					ui_disp[3]=m%10;
				}
				if (ts>=sRTC_GetTS()) break;
				rt_event_send(f_led,LED_RED);
				
				state=1;
				rt_event_recv(f_key,F_KEY_31|F_KEY_33,RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR,0,&e);
				while (rt_event_recv(f_key,F_KEY_31|F_KEY_33,RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR,RT_TICK_PER_SECOND/2,&e)!=RT_EOK)
				{
					state=!state;
					rt_event_send(f_led,state?LED_RED:LED_BLACK);
				}
				if (e&F_KEY_33) break;
				rt_event_send(f_led,LED_RED);
			}
			rt_event_send(f_led,LED_BLACK);
			rt_mutex_release(m_display);
		}
	}
}
