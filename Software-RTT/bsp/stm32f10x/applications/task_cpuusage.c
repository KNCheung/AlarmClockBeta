#include <rtthread.h>
#include <application.h>
#include <board.h>

#ifdef  RT_USING_COMPONENTS_INIT
#include <components.h>
#endif  /* RT_USING_COMPONENTS_INIT */

//#define DEBUGMODE

#ifdef DEBUGMODE
#define CPU_USAGE_CALC_TICK	RT_TICK_PER_SECOND/2
#define CPU_USAGE_LOOP 100
rt_tick_t total_cnt=0,tick,cnt;
volatile uint32_t loop;
uint8_t print_cpu_usage;
void idle_hook(void)
{
	if (total_cnt == 0)
	{
		rt_enter_critical();
		print_cpu_usage = 0;
		tick = rt_tick_get();
		while (rt_tick_get() - tick < CPU_USAGE_CALC_TICK)
		{
			total_cnt++;
			loop = 0;
			while (loop < CPU_USAGE_LOOP)
				loop++;
		}
		rt_exit_critical();
	}
	else
	{
		cnt = 0;
		tick = rt_tick_get();
		while (rt_tick_get()-tick < CPU_USAGE_CALC_TICK)
		{
			cnt++;
			loop = 0;
			while (loop<CPU_USAGE_LOOP)
				loop++;
		}
		if (print_cpu_usage)
		{
			print_cpu_usage = 0;
			tick = ((total_cnt-cnt)*1000/total_cnt);
			if (tick>1000)
			{
				tick = 0;
				total_cnt = cnt;
			}
			rt_kprintf("\nCPU Usage: %d.%d%%\n",tick/10,tick%10);
		}
	}
}	
void cpu_usage(void)
{
	print_cpu_usage = 1;
	return;
}
FINSH_FUNCTION_EXPORT(cpu_usage,cpu usage)
#else
extern char ADC_Running;
void idle_hook(void)
{
	if (!ADC_Running) 
		__asm("WFI");
}
#endif
