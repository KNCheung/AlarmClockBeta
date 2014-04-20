#include <rtthread.h>
#include <board.h>
#include <application.h>

#include <REG.h>
#include <IIC.h>

extern uint8_t clock_h, clock_m, clock_s;
extern uint32_t reg_output[REG_SCREEN_NUMBER];
extern rt_event_t en_event, reg_event;
uint64_t start_tim, curr_tim, interval;

void rt_thread_counter_entry(void* parameter)
{
	rt_uint32_t e;
	while (1)
	{
		rt_event_recv(en_event, EVENT_COUNTER, RT_EVENT_FLAG_CLEAR | RT_EVENT_FLAG_OR, RT_WAITING_FOREVER, &e);
		start_tim = clock_h*3600+clock_m*60+clock_s;
		rt_event_send(reg_event, REG_COUNTER_MSK);
		do
		{
			curr_tim = clock_h*3600+clock_m*60+clock_s;
			interval = (curr_tim+86400-start_tim)%86400;
			if (interval<1800)
			{
				reg_output[REG_COUNTER] = REG_Convert(REG_HexToReg(interval%10),
				                                      REG_HexToReg((interval%60)/10),
				                                      REG_HexToReg((interval/60)%10),
				                                      REG_HexToReg((interval/60)%60/10),
				                                      1,0);
			}else{
				reg_output[REG_COUNTER] = REG_Convert(REG_HexToReg((interval/60)%10),
				                                      REG_HexToReg((interval/60)%60/10),
				                                      REG_HexToReg((interval/3600)%10),
				                                      REG_HexToReg((interval/3600)/10),
				                                      1,0);
			}
		}while(rt_event_recv(en_event, EVENT_COUNTER, RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR, RT_TICK_PER_SECOND*1, &e));
    while (1)
    {
      if (rt_event_recv(en_event, EVENT_COUNTER, RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR, RT_TICK_PER_SECOND/2, &e)==RT_EOK)
        break;
      REG_Off();
      if (rt_event_recv(en_event, EVENT_COUNTER, RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR, RT_TICK_PER_SECOND/2, &e)==RT_EOK)
        break;
      REG_On();
    }
    REG_On();
		rt_event_recv(reg_event, REG_COUNTER_MSK, RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR, 0, &e);
	}
}
