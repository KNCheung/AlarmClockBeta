#include <board.h>
#include <rtthread.h>
#include <application.h>

#include <stdlib.h>

rt_uint8_t doge_stack[256];
struct rt_thread doge_thread;

void task_doge_init(void);
void rt_thread_doge_entry(void* parameter);

void task_doge_init(void)
{
	rt_err_t result;
	
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	IWDG_SetPrescaler(IWDG_Prescaler_256); //40kHz/256=156.25Hz
	IWDG_SetReload(312);	//156.25Hz/312=0.5Hz
	IWDG_ReloadCounter();	//Initialize
	
	result = rt_thread_init(&doge_thread,
	                        "doge",
	                        rt_thread_doge_entry,
	                        RT_NULL,
	                        (rt_uint8_t*)doge_stack,
	                        sizeof(doge_stack),
	                        PRIO_DOGE,
	                        2);
	if (result == RT_EOK) rt_thread_startup(&doge_thread);
	IWDG_Enable();
}

void rt_thread_doge_entry(void* parameter)
{
	while(1)
	{
		IWDG_ReloadCounter();
		rt_thread_delay_hmsm(0,0,0,500);
	}
}
