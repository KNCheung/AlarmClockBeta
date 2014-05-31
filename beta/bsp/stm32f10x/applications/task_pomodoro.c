#include <board.h>
#include <rtthread.h>
#include <application.h>

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
//	if (result == RT_EOK) rt_thread_startup(&pomodoro_thread);
}


void rt_thread_pomodoro_entry(void* parameter)
{
}
