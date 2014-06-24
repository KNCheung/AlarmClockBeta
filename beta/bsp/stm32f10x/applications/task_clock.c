#include <board.h>
#include <rtthread.h>
#include <application.h>

#include <stdlib.h>

rt_uint8_t clock_stack[256];
static struct rt_timer timer_add_second;
struct rt_thread clock_thread;

void task_clock_init(void);
void rt_thread_clock_entry(void* parameter);

void refresh_rtc(void);

uint8_t rtc_Y,rtc_M,rtc_D,rtc_h,rtc_m,rtc_s;

void timer_add_second_entry(void *parameter)
{
	rtc_s++;
	rtc_s%=60;
}

void task_clock_init(void)
{
	rt_err_t result;
	result = rt_thread_init(&clock_thread,
	                        "Clock",
	                        rt_thread_clock_entry,
	                        RT_NULL,
	                        (rt_uint8_t*)clock_stack,
	                        sizeof(clock_stack),
	                        PRIO_CLOCK,
	                        2);
	rt_timer_init(&timer_add_second,"Add Sec",timer_add_second_entry,RT_NULL,RT_TICK_PER_SECOND,RT_TIMER_FLAG_PERIODIC|RT_TIMER_FLAG_SOFT_TIMER);
	if (result == RT_EOK) rt_thread_startup(&clock_thread);
}


void rt_thread_clock_entry(void* parameter)
{
	static const rt_tick_t tick_one_second = RT_TICK_PER_SECOND;
	refresh_rtc();
	srand(rtc_m*60+rtc_s+rtc_h*3600);
	rt_thread_delay_hmsm(0,0,1,0);
	rt_timer_start(&timer_add_second);
	while(1)
	{
		refresh_rtc();
		rt_timer_control(&timer_add_second,RT_TIMER_CTRL_SET_TIME,(void*)&tick_one_second);
		rt_thread_delay_hmsm(0,0,(60-rtc_s),0);
	}
}

void refresh_rtc(void)
{
	uint8_t h,m,s,Y,M,D;
	rt_enter_critical();
	h=IIC_Read(DS3231_ADDRESS, DS3231_HOUR);
	m=IIC_Read(DS3231_ADDRESS, DS3231_MINUTE);
	s=IIC_Read(DS3231_ADDRESS, DS3231_SECOND);
	Y=IIC_Read(DS3231_ADDRESS, DS3231_YEAR);
	M=IIC_Read(DS3231_ADDRESS, DS3231_MONTH);
	D=IIC_Read(DS3231_ADDRESS, DS3231_DAY);
	rt_exit_critical();
	rtc_h=(h>>4)*10+h%16;
	rtc_m=(m>>4)*10+m%16;
	rtc_s=(s>>4)*10+s%16;
	rtc_Y=(Y>>4)*10+Y%16;
	rtc_M=(M>>4)%2*10+M%16;
	rtc_D=(D>>4)*10+D%16;
}
