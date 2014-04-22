#include <rtthread.h>
#include <application.h>
#include <board.h>

#include <REG.h>

extern rt_event_t en_event;
extern rt_event_t reg_event;
extern uint8_t clock_s;

uint32_t reg_output[REG_SCREEN_NUMBER];

void rt_thread_reg_entry(void* parameter)
{
  rt_uint32_t e;
  uint8_t i;
  REG_Init();
  REG_On();
  for (i=0;i<REG_SCREEN_NUMBER;i++)
    reg_output[i] = 0XBF;
  rt_event_send(en_event,EVENT_REG);
  reg_event = rt_event_create("REGManag",RT_IPC_FLAG_FIFO);
  while (1)
  {
    rt_event_recv(en_event,EVENT_REG,RT_EVENT_FLAG_AND,RT_WAITING_FOREVER,&e);
    rt_event_recv(reg_event,0xFFFFFFFF,RT_EVENT_FLAG_OR,RT_WAITING_FOREVER,&e);
    i=0;
    while ((e%2)==0)
    {
    	i++;
    	e>>=1;
    }
    REG_Set(reg_output[i]);
    REG_Update();
    rt_thread_delay(2);
  }
}

void ToggleREG(void)
{
	rt_uint32_t e;
	if (REG_State())
		{
    	REG_Off();
    	rt_event_recv(en_event,EVENT_REG, RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR,0,&e);
    	rt_kprintf("\n  REG is Turned Off\n");
  	}else{
    	rt_event_send(en_event,EVENT_REG);
    	rt_thread_delay(20);
    	REG_On();
    	rt_kprintf("\n  REG is Turned On\n");
  	}
}
