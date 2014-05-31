#include <board.h>
#include <rtthread.h>
#include <application.h>
#include <stdlib.h>

rt_uint8_t counter_stack[256];
struct rt_thread counter_thread;

void task_counter_init(void);
void rt_thread_counter_entry(void* parameter);

void task_counter_init(void)
{
	rt_err_t result;
	result = rt_thread_init(&counter_thread,
	                        "Counter",
	                        rt_thread_counter_entry,
	                        RT_NULL,
	                        (rt_uint8_t*)counter_stack,
	                        sizeof(counter_stack),
	                        PRIO_COUNTER,
	                        2);
	if (result == RT_EOK) rt_thread_startup(&counter_thread);
}


void rt_thread_counter_entry(void* parameter)
{
	extern rt_event_t f_en,f_key;
	extern rt_mutex_t m_reg;
	extern uint8_t ui_disp[4];
	uint32_t ts;
	rt_uint32_t e;
	uint8_t state,h,m,s;
	while (1)
	{
		rt_event_recv(f_en,F_EN_COUNTER,RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR ,RT_WAITING_FOREVER,&e);
		if (rt_mutex_take(m_reg,0)==RT_EOK)
		{
			ts=sRTC_GetTS();
			ui_disp[0]=0;
			ui_disp[1]=0x80;
			state=0x80;
			ui_disp[2]=0;
			ui_disp[3]=0;
			rt_event_recv(f_key,F_KEY_31|F_KEY_33,RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,0,&e);
			while (1)
			{
				while (rt_event_recv(f_key,F_KEY_31|F_KEY_33,RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,RT_TICK_PER_SECOND/2,&e)!=RT_EOK)
				{
					sRTC_GetRTC(sRTC_GetTS()-ts,NULL,NULL,NULL,&h,&m,&s);
					if ((m<15)&&(h==0))
					{
						ui_disp[0]=(m/10);
						ui_disp[1]=0x80|(m%10);
						ui_disp[2]=s/10;
						ui_disp[3]=s%10;
					}else{
						ui_disp[0]=(h/10);
						ui_disp[1]=0x80|(h%10);
						ui_disp[2]=m/10;
						ui_disp[3]=m%10;
					}
				}
				if (e&F_KEY_33) break;
				else 
				{
					state|=0x01;
					while (rt_event_recv(f_key,F_KEY_31|F_KEY_33,RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR,RT_TICK_PER_SECOND/2,&e)!=RT_EOK)
					{
						state^=0x01;
						PushREG(REG2,REG_Shut,state);
					}
					PushREG(REG2,REG_Shut,0x01);
					if (e&F_KEY_33) break;
				}
			}
			rt_mutex_release(m_reg);
		}
		rt_event_recv(f_en,F_EN_COUNTER,RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR ,0,&e);
	}
}
