#include <board.h>
#include <rtthread.h>
#include <application.h>

rt_uint8_t clock_stack[256];
struct rt_thread clock_thread;

void task_clock_init(void);
void rt_thread_clock_entry(void* parameter);

uint8_t rtc_Y,rtc_M,rtc_D,rtc_h,rtc_m,rtc_s;

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
	if (result == RT_EOK) rt_thread_startup(&clock_thread);
}


void rt_thread_clock_entry(void* parameter)
{
	uint8_t h,m,s,Y,M,D;
	rt_thread_delay_hmsm(0,0,1,0);
	while(1)
	{
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
		PushREG(REG2,REG_X1,rtc_h/10);
		PushREG(REG2,REG_X2,(rtc_h%10)|0x80);
		PushREG(REG2,REG_X3,rtc_m/10);
		PushREG(REG2,REG_X4,rtc_m%10);
		rt_thread_delay_hmsm(0,0,(60-rtc_s),0);
	}
}
