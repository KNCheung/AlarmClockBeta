#include <rtthread.h>
#include <board.h>
#include <application.h>

#include <ADC.h>
#include <REG.h>

extern uint32_t reg_output[REG_SCREEN_NUMBER]; 
extern rt_event_t reg_event;
uint8_t voltage;

void rt_thread_protect_entry(void* parameter)
{
  rt_uint32_t e;
  voltage = GetVoltage();
  reg_output[REG_PROTECT]=REG_Convert(0x63,REG_HexToReg(voltage%10),REG_HexToReg(voltage/10%10),REG_HexToReg(voltage/100%10),0,1);
  rt_event_send(reg_event,REG_PROTECT_MSK);
  rt_thread_delay(RT_TICK_PER_SECOND*1);
  rt_event_recv(reg_event,REG_PROTECT_MSK,RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR,0,&e);
  while (1)
  {
    if (voltage<=108)
    {
      while (1)
      {
        rt_event_send(reg_event,REG_PROTECT_MSK);
        reg_output[REG_PROTECT]=REG_Convert(0x63,REG_HexToReg(voltage%10),REG_HexToReg(voltage/10%10),REG_HexToReg(voltage/100%10),0,1);
        voltage = GetVoltage();
        rt_thread_delay(RT_TICK_PER_SECOND*2/3);
        rt_event_recv(reg_event,REG_PROTECT_MSK,RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR,0,&e);
        rt_thread_delay(RT_TICK_PER_SECOND/3);
      }
    }
    rt_thread_delay(RT_TICK_PER_SECOND*60);
  }
}
