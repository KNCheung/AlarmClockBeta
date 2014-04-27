#include <rtthread.h>
#include <board.h>
#include <application.h>

#include <REG.h>
#include <DS18B20.h>

extern uint32_t reg_output[REG_SCREEN_NUMBER]; 
extern rt_event_t en_event,reg_event;

void rt_thread_temp_entry(void* parameter)
{
  rt_uint32_t e;
  rt_err_t err;
  int16_t temp;
  OneWire_Init();
  rt_enter_critical();
  DS18B20_StartConvTemp();
  rt_exit_critical();
  rt_thread_delay_hmsm(0,0,0,500);
  while (1)
  {
    err = rt_event_recv(en_event,EVENT_TEMP,RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR,RT_TICK_PER_SECOND*180,&e);
	  
	if (err == RT_EOK)
	{
		rt_event_send(reg_event,REG_TEMP_MSK);
		reg_output[REG_TEMP]=0xBFBFBFBF;
	}
    
    rt_enter_critical();
    DS18B20_StartConvTemp();
    rt_exit_critical();
    
	if (err == RT_EOK)
	{
		rt_thread_delay_hmsm(0,0,0,750);
    
		rt_enter_critical();
		temp = (int16_t)(DS18B20_ReadTemp()*10+0.5f);
		rt_exit_critical();
		reg_output[REG_TEMP]=REG_Convert(0x27,REG_HexToReg(temp%10),REG_HexToReg(temp/10%10),REG_HexToReg(temp/100),0,1);
    
		rt_thread_delay_hmsm(0,0,1,0);
		WAIT_FOR_RELEASE;
		rt_event_recv(reg_event,REG_TEMP_MSK,RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR,0,&e);
	}
  }
}
