/*
 * File      : application.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      the first version
 * 2013-07-12     aozima       update for auto initial.
 */

/**
 * @addtogroup STM32
 */
/*@{*/
 
#include <board.h>
#include <rtthread.h>


#ifdef  RT_USING_COMPONENTS_INIT
#include <components.h>
#endif  /* RT_USING_COMPONENTS_INIT */

#include <application.h>
#include <task_temp.h>
#include <task_ir.h>
#include <task_reg.h>
#include <task_pomodoro.h>
#include <task_date.h>
#include <task_clock.h>
#include <task_counter.h>
#include <task_protect.h>

#include <IIC.h>
#include <REG.h>
#include <ADC.h>

ALIGN(RT_ALIGN_SIZE)

static rt_uint8_t reg_stack[256];
static struct rt_thread reg_thread;

static rt_uint8_t clock_stack[256];
static struct rt_thread clock_thread;
rt_timer_t update_clock_timer,update_second_timer;

static rt_uint8_t ir_stack[256];
static struct rt_thread ir_thread;

static rt_uint8_t temp_stack[256];
static struct rt_thread temp_thread;

static rt_uint8_t date_stack[256];
static struct rt_thread date_thread;

static rt_uint8_t pomodoro_stack[256];
static struct rt_thread pomodoro_thread;

static rt_uint8_t counter_stack[256];
static struct rt_thread counter_thread;

static rt_uint8_t protect_stack[256];
static struct rt_thread protect_thread;

	rt_thread_t init_thread;

rt_mq_t ir_mq = RT_NULL;
rt_event_t en_event = RT_NULL;
rt_event_t reg_event = RT_NULL;

extern uint8_t clock_s;

void lp(void);

uint8_t Setting[SETTING_LENGTH];
void Setting_Read(void);
void Setting_Write(void);

extern uint32_t reg_output[REG_SCREEN_NUMBER];

void rt_init_thread_entry(void* parameter)
{
	rt_err_t result;
	uint8_t t;

#ifdef RT_USING_COMPONENTS_INIT
	/* initialization RT-Thread Components */
	rt_components_init();
#endif

#ifdef  RT_USING_FINSH
	finsh_set_device(RT_CONSOLE_DEVICE_NAME);
#endif  /* RT_USING_FINSH */
	
	result = rt_thread_init(&reg_thread,
	                        "REG",
	                        rt_thread_reg_entry,
	                        RT_NULL,
	                        (rt_uint8_t*)reg_stack,
	                        sizeof(reg_stack),
	                        PRIO_REG,
	                        2);
	if (result == RT_EOK) rt_thread_startup(&reg_thread);
	
  	result = rt_thread_init(&protect_thread,
	                        "Protect",
	                        rt_thread_protect_entry,
	                        RT_NULL,
	                        (rt_uint8_t*)protect_stack,
	                        sizeof(protect_stack),
	                        PRIO_PROTECT,
	                        5);
	if (result == RT_EOK) rt_thread_startup(&protect_thread);
  
	IIC_Init();
	IIC_Write(DS3231_ADDRESS, DS3231_CON_STA,0x80);
	do
	{
	  Setting_Read();
	  rt_thread_delay_hmsm(0,0,0,10);
	}while(Setting[0]!=42);
	
	clock_s = IIC_Read(DS3231_ADDRESS, DS3231_SECOND);
	t=clock_s;
	while (t==clock_s)
	{
	  rt_thread_delay_hmsm(0,0,0,10);
	  clock_s = IIC_Read(DS3231_ADDRESS, DS3231_SECOND);
	}
	update_second_timer = rt_timer_create("Second",update_second,RT_NULL,RT_TICK_PER_SECOND,RT_TIMER_FLAG_HARD_TIMER|RT_TIMER_FLAG_PERIODIC|RT_TIMER_FLAG_ACTIVATED);
	rt_timer_start(update_second_timer);
	  

	result = rt_thread_init(&clock_thread,
	                        "Clock",
	                        rt_thread_clock_entry,
	                        RT_NULL,
	                        (rt_uint8_t*)clock_stack,
	                        sizeof(clock_stack),
	                        PRIO_CLOCK,
	                        2);
	if (result == RT_EOK) rt_thread_startup(&clock_thread);
	
	result = rt_thread_init(&ir_thread,
	                        "IR",
	                        rt_thread_ir_entry,
	                        RT_NULL,
	                        (rt_uint8_t*)ir_stack,
	                        sizeof(ir_stack),
	                        PRIO_IR,
	                        5);
	if (result == RT_EOK) rt_thread_startup(&ir_thread);
	
	result = rt_thread_init(&temp_thread,
	                        "Temp",
	                        rt_thread_temp_entry,
	                        RT_NULL,
	                        (rt_uint8_t*)temp_stack,
	                        sizeof(temp_stack),
	                        PRIO_TEMP,
	                        5);
	if (result == RT_EOK) rt_thread_startup(&temp_thread);
	
	result = rt_thread_init(&date_thread,
	                        "Date",
	                        rt_thread_date_entry,
	                        RT_NULL,
	                        (rt_uint8_t*)date_stack,
	                        sizeof(date_stack),
	                        PRIO_DATE,
	                        5);
	if (result == RT_EOK) rt_thread_startup(&date_thread);
	
	result = rt_thread_init(&pomodoro_thread,
	                        "Pomodoro",
	                        rt_thread_pomodoro_entry,
	                        RT_NULL,
	                        (rt_uint8_t*)pomodoro_stack,
	                        sizeof(pomodoro_stack),
	                        PRIO_POMODORO,
	                        5);
	if (result == RT_EOK) rt_thread_startup(&pomodoro_thread);
	
	result = rt_thread_init(&counter_thread,
	                        "Counter",
	                        rt_thread_counter_entry,
	                        RT_NULL,
	                        (rt_uint8_t*)counter_stack,
	                        sizeof(counter_stack),
	                        PRIO_COUNTER,
	                        5);
	if (result == RT_EOK) rt_thread_startup(&counter_thread);
	
  rt_thread_delete(init_thread);
  while (1);
}

int rt_task_object_init(void)
{
	en_event = rt_event_create("Enable",RT_IPC_FLAG_FIFO);
    reg_event = rt_event_create("REGManag",RT_IPC_FLAG_FIFO);
	ir_mq = rt_mq_create("IR",sizeof(uint16_t),64,RT_IPC_FLAG_FIFO);
	return 0;
}

int rt_application_init(void)
{
	init_thread = rt_thread_create("init",
	                               rt_init_thread_entry, RT_NULL,
	                               2048, RT_THREAD_PRIORITY_MAX-2, 1);

	if (init_thread != RT_NULL)
	    rt_thread_startup(init_thread);

	return 0;
}

void reboot(void)
{
	rt_kprintf("=====REBOOTED=====");
	REG_Off();
	rt_thread_delay_hmsm(0,0,0,10);  
	NVIC_SystemReset();
	return;
}
FINSH_FUNCTION_EXPORT(reboot,Reboot the MCU)

void fnDebug(uint8_t a,uint8_t b,uint8_t c)
{
  rt_kprintf("%lx\n",TIM_GetCounter(TIM3));
}
FINSH_FUNCTION_EXPORT_ALIAS(fnDebug,debug,Debug Function)

void Print_Setting(void)
{
	uint8_t i;
	for (i=0;i<SETTING_LENGTH;i++)
	{
		if (i%8==0)
			rt_kprintf("\n");
		rt_kprintf("\t%d ",Setting[i]);
	}
	rt_kprintf("\n");
	return;
}
FINSH_FUNCTION_EXPORT_ALIAS(Print_Setting,setting,Print Setting Values)

void Setting_Write(void)
{
	uint8_t t;
	while (1)
	{
	IIC_WriteSeq(AT24C32_ADDRESS,SETTING_BASE,Setting,SETTING_LENGTH);
	rt_thread_delay_hmsm(0,0,0,10);
	IIC_ReadSeq(AT24C32_ADDRESS,SETTING_BASE+1, &t,1);
	if (t!=Setting[1])
		rt_thread_delay_hmsm(0,0,0,10);
	else
		break;
	}
	return;
}

void Setting_Read(void)
{
	IIC_ReadSeq(AT24C32_ADDRESS,SETTING_BASE,Setting,SETTING_LENGTH);
	return;
}

void pomodoro_set(uint8_t a,uint8_t b,uint8_t c)
{
	Setting[POMODORO_TIME] = a;
	Setting[POMODORO_BREAK_TIME] = b;
	Setting[POMODORO_REST_TIME] = c;
	Setting_Write();
	return;
}
FINSH_FUNCTION_EXPORT_ALIAS(pomodoro_set,pomodoro,Set Interval of Pomodoro Clock(Working,Break,Rest))

void time(void)
{
	rt_enter_critical();
	rt_kprintf("         Date:\t20%x-%x-%x\n         Time:\t%x:%x:%x\n  Temperature:\t%d C\n",\
	                      IIC_Read(DS3231_ADDRESS, DS3231_YEAR),IIC_Read(DS3231_ADDRESS,DS3231_MONTH)&0x7f,IIC_Read(DS3231_ADDRESS,DS3231_DAY),\
	                      IIC_Read(DS3231_ADDRESS, DS3231_HOUR),IIC_Read(DS3231_ADDRESS,DS3231_MINUTE),IIC_Read(DS3231_ADDRESS,DS3231_SECOND),\
	                      IIC_Read(DS3231_ADDRESS, DS3231_TEMP_MSB));
	rt_exit_critical();
	return;
}
FINSH_FUNCTION_EXPORT(time,Read DS3231)

void lp(void)
{
	REG_Off();
	NVIC_SystemLPConfig(NVIC_LP_SEVONPEND,ENABLE);
	PWR_WakeUpPinCmd(ENABLE);
	PWR_EnterSTANDBYMode();
	return;
}
FINSH_FUNCTION_EXPORT(lp , Low Power Mode)

void Set_Time(uint8_t Y,uint8_t M,uint8_t D,uint8_t w, uint8_t h,uint8_t m, uint8_t s)
{
  IIC_Write(DS3231_ADDRESS, DS3231_SECOND , ((s/10)<<4)|(s%10));
  IIC_Write(DS3231_ADDRESS, DS3231_MINUTE , ((m/10)<<4)|(m%10));
  IIC_Write(DS3231_ADDRESS, DS3231_HOUR , ((h/10)<<4)|(h%10));
  IIC_Write(DS3231_ADDRESS, DS3231_WEEKDAY , ((w/10)<<4)|(w%10));
  IIC_Write(DS3231_ADDRESS, DS3231_YEAR , ((Y/10)<<4)|(Y%10));
  IIC_Write(DS3231_ADDRESS, DS3231_MONTH , ((M/10)<<4)|(M%10));
  IIC_Write(DS3231_ADDRESS, DS3231_DAY , ((D/10)<<4)|(D%10));  
  time();
  return;
}
FINSH_FUNCTION_EXPORT(Set_Time , Set the time store in the DS3231)

extern uint8_t voltage;
FINSH_VAR_EXPORT(voltage, finsh_type_uchar, The Power Voltage)
