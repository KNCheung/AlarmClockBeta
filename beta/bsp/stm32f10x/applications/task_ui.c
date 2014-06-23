#include <board.h>
#include <rtthread.h>
#include <application.h>

#include <stdlib.h>

rt_uint8_t ui_stack[256];
uint8_t ui_disp[4]={0x0A,0x0A,0x0A,0x0A};
struct rt_thread ui_thread;

void task_ui_init(void);
void rt_thread_ui_entry(void* parameter);

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
	extern rt_event_t f_msg,f_en,f_key;
	extern uint8_t emoticon[][8];
	extern uint8_t rtc_Y,rtc_M,rtc_D,rtc_h,rtc_m,rtc_s;
	extern int16_t temperature,humidity,outdoortemperature;
	extern rt_mutex_t m_display;
	
	rt_thread_delay_hmsm(0,0,1,0);
    while (rt_mq_recv(mq_ir,&code,sizeof(uint32_t),0)==RT_EOK);
	rt_event_recv(f_en,0xFFFFFFFF,RT_EVENT_FLAG_CLEAR|RT_EVENT_FLAG_OR,0,NULL);
	while (1)
	{
		if (rt_mq_recv(mq_ir,&code,sizeof(uint32_t),RT_TICK_PER_SECOND/2)==RT_EOK)
			switch (code%256)
			{
				case 0xA2:	rt_event_send(f_key,F_KEY_11); rt_event_send(f_msg,F_ANI_TIME);	break;
				case 0x62:	rt_event_send(f_key,F_KEY_12); rt_event_send(f_msg,F_ANI_DATE);	break;
				case 0xE2:	rt_event_send(f_key,F_KEY_13); rt_event_send(f_msg,F_ANI_TEMP);	break;
				case 0x22:	rt_event_send(f_key,F_KEY_21); rt_event_send(f_msg,F_ANI_PREV);	break;
				case 0x02:	rt_event_send(f_key,F_KEY_22); rt_event_send(f_msg,F_ANI_NEXT);	break;
				case 0xC2:	rt_event_send(f_key,F_KEY_23); rt_event_send(f_msg,F_ANI_CHOOSE);	break;
				case 0xE0:	rt_event_send(f_key,F_KEY_31); break;
				case 0xA8:	rt_event_send(f_key,F_KEY_32); break;
				case 0x90:  rt_event_send(f_key,F_KEY_33); break;
				case 0x68:	rt_event_send(f_key,F_KEY_41); break;
				case 0x98:	rt_event_send(f_key,F_KEY_42); break;
				case 0xB0:  rt_event_send(f_key,F_KEY_43); break;
				case 0x30:	rt_event_send(f_key,F_KEY_51); break;
				case 0x18:	rt_event_send(f_key,F_KEY_52); break;
				case 0x7A:  rt_event_send(f_key,F_KEY_53); break;
				case 0x10:	rt_event_send(f_key,F_KEY_61); break;
				case 0x38:	rt_event_send(f_key,F_KEY_62); break;
				case 0x5A:  rt_event_send(f_key,F_KEY_63); break;
				case 0x52:	
					PushREG(REGA,REG_Shut,0x00,0x00);
					LED_RGB(0,0,255);
					while(1);
				default:
					break;
			}
		if (rt_mutex_take(m_display,0)==RT_EOK)
		{
			PushREG(REG2,REG_X1,rtc_h/10);
			PushREG(REG2,REG_X2,0x80|(rtc_h%10));
			PushREG(REG2,REG_X3,rtc_m/10);
			PushREG(REG2,REG_X4,rtc_m%10);
			rt_mutex_release(m_display);
		}else{
			PushREG(REG2,REG_X1,ui_disp[0]);
			PushREG(REG2,REG_X2,ui_disp[1]);
			PushREG(REG2,REG_X3,ui_disp[2]);
			PushREG(REG2,REG_X4,ui_disp[3]);
		}
		emoticon[ICON_BLANK][0]=(0x10)*(rtc_M/10%10)+(0x01)*(rtc_M%10);
		emoticon[ICON_BLANK][1]=(0x10)*(rtc_D/10%10)+(0x01)*(rtc_D%10);
		emoticon[ICON_BLANK][2]=(0x10)*(rtc_h/10%10)+(0x01)*(rtc_h%10);
		emoticon[ICON_BLANK][3]=(0x10)*(rtc_m/10%10)+(0x01)*(rtc_m%10);
		emoticon[ICON_BLANK][4]=0;
		emoticon[ICON_BLANK][5]=(0x10)*((temperature+5)/100%10)+(0x01)*((temperature+5)/10%10);
		emoticon[ICON_BLANK][6]=(0x10)*((outdoortemperature+5)/100%10)+(0x01)*((outdoortemperature+5)/10%10);
		emoticon[ICON_BLANK][7]=(0x10)*((humidity+5)/100%10)+(0x01)*((humidity+5)/10%10);
		Transposition(emoticon[ICON_BLANK],emoticon[ICON_BLANK]);
	}
}
