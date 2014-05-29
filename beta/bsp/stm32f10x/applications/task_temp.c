#include <board.h>
#include <rtthread.h>
#include <application.h>

#include <stdlib.h>

rt_uint8_t temp_stack[256];
struct rt_thread temp_thread;

void task_temp_init(void);
void rt_thread_temp_entry(void* parameter);

int16_t temperature,humidity,outdoortemperature;

void task_temp_init(void)
{
	rt_err_t result;
	rt_enter_critical();
//	DS18B20_StartConvTemp();
	rt_exit_critical();
	result = rt_thread_init(&temp_thread,
	                        "temperature",
	                        rt_thread_temp_entry,
	                        RT_NULL,
	                        (rt_uint8_t*)temp_stack,
	                        sizeof(temp_stack),
	                        PRIO_TEMP,
	                        2);
	if (result == RT_EOK) rt_thread_startup(&temp_thread);
}

void rt_thread_temp_entry(void* parameter)
{
	int16_t t,h;
	rt_thread_delay_hmsm(0,0,1,0);
	while(1)
	{
		rt_enter_critical();
	//	DS18B20_StartConvTemp();
		rt_exit_critical();
		while (AM2302_Read(&h,&t)==ERROR)
		{
			t=999;h=000;
			rt_thread_delay_hmsm(0,0,2,500);
		}
		temperature=t;
		humidity=h;
		rt_thread_delay_hmsm(0,0,3,0);
		rt_enter_critical();
	//	outdoortemperature=(int16_t)(DS18B20_ReadTemp()*10);
		rt_exit_critical();
		rt_thread_delay_hmsm(0,0,10,0);
	}
}
