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
#include <math.h>

#ifdef  RT_USING_COMPONENTS_INIT
#include <components.h>
#endif  /* RT_USING_COMPONENTS_INIT */

#include <application.h>

#define RELEASE

ALIGN(RT_ALIGN_SIZE)
rt_thread_t init_thread;

rt_mutex_t 	m_reg		=	NULL;
rt_mq_t 	mq_ir		=	NULL;
rt_event_t 	f_msg		=	NULL;
rt_event_t	f_en		=	NULL;
rt_event_t	f_key		=	NULL;
rt_event_t  f_led		=	NULL;
rt_mutex_t	m_led		=	NULL;

void idle_hook(void);

void idle_hook(void)
{
#ifdef RELEASE
	__asm("WFI");
#endif
}

void rt_init_thread_entry(void* parameter)
{
	rt_thread_idle_sethook(idle_hook);
	
#ifdef RT_USING_COMPONENTS_INIT
	/* initialization RT-Thread Components */
	rt_components_init();
#endif

#ifdef  RT_USING_FINSH
	finsh_set_device(RT_CONSOLE_DEVICE_NAME);
#endif  /* RT_USING_FINSH */
	
  task_clock_init();
  task_temp_init();
  task_animation_init();
  task_ui_init();
  task_counter_init();
  task_pomodoro_init();
  task_led_init();
	
#ifdef RELEASE
  task_doge_init();
#endif

  rt_thread_delete(init_thread);
  while (1);
}

int rt_task_object_init(void)
{
	m_reg = rt_mutex_create("REG",RT_IPC_FLAG_FIFO);
	m_led = rt_mutex_create("LED",RT_IPC_FLAG_FIFO);
	mq_ir = rt_mq_create("IR",sizeof(uint16_t),64,RT_IPC_FLAG_FIFO);
	f_msg = rt_event_create("Msg",RT_IPC_FLAG_FIFO);
	f_en  = rt_event_create("ENABLE",RT_IPC_FLAG_FIFO);
	f_key = rt_event_create("App Key",RT_IPC_FLAG_FIFO);
	f_led = rt_event_create("LED",RT_IPC_FLAG_FIFO);
	return 0;
}

int rt_application_init(void)
{
	refresh_rtc();
	init_thread = rt_thread_create("init",
	                               rt_init_thread_entry, RT_NULL,
	                               2048, RT_THREAD_PRIORITY_MAX-2, 1);

	if (init_thread != RT_NULL)
	    rt_thread_startup(init_thread);
	return 0;
}
