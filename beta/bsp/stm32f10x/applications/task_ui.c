#include <board.h>
#include <rtthread.h>
#include <application.h>

#include <stdlib.h>

rt_uint8_t ui_stack[256];
struct rt_thread ui_thread;

void task_ui_init(void);
void rt_thread_ui_entry(void* parameter);

extern int16_t temperature,humidity;

extern rt_mq_t mq_ir;

void task_ui_init(void)
{
	rt_err_t result;
	result = rt_thread_init(&ui_thread,
	                        "UI",
	                        rt_thread_ui_entry,
	                        RT_NULL,
	                        (rt_uint8_t*)ui_stack,
	                        sizeof(ui_stack),
	                        PRIO_UI,
	                        2);
	if (result == RT_EOK) rt_thread_startup(&ui_thread);
}

void rt_thread_ui_entry(void* parameter)
{
	uint32_t code;
	extern rt_event_t f_msg;
	extern uint8_t emoticon[][8];
	rt_thread_delay_hmsm(0,0,1,0);
    while (rt_mq_recv(mq_ir,&code,sizeof(uint32_t),0)==RT_EOK);
	while (1)
	{
		rt_mq_recv(mq_ir,&code,sizeof(uint32_t),RT_WAITING_FOREVER);
		switch (code%256)
		{
			case 0xA2:	rt_event_send(f_msg,F_ANI_TIME);	break;
			case 0x62:	rt_event_send(f_msg,F_ANI_DATE);	break;
			case 0xE2:	rt_event_send(f_msg,F_ANI_TEMP);	break;
			case 0x22:	rt_event_send(f_msg,F_ANI_PREV);	break;
			case 0x02:	rt_event_send(f_msg,F_ANI_NEXT);	break;
			case 0xC2:	rt_event_send(f_msg,F_ANI_CHOOSE);	break;
			case 0x5A:	break;
			case 0x52:	
				PushBitMap(REG1,emoticon[ICON_JIONG]);
				PushREG(REG2,REG_X1,0x0C);
				PushREG(REG2,REG_X2,0x0B);
				PushREG(REG2,REG_X3,0x0D);
				PushREG(REG2,REG_X4,0x0E);
				while(1);
			default:
				break;
		}
	}
}
