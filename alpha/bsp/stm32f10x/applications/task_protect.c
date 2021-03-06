#include <rtthread.h>
#include <board.h>
#include <application.h>

#include <ADC.h>
#include <REG.h>

extern uint32_t reg_output[REG_SCREEN_NUMBER]; 
extern rt_event_t reg_event,en_event;
uint8_t voltage;
extern struct rt_thread protect_thread;

void rt_thread_protect_entry(void* parameter)
{
  rt_uint32_t e;
  voltage = GetVoltage();
  if (voltage<3.0) 
	  reg_output[REG_PROTECT]=0xABE3F9F9;
  else
	reg_output[REG_PROTECT]=REG_Convert(0x63,REG_HexToReg(voltage%10),REG_HexToReg(voltage/10%10),REG_HexToReg(voltage/100%10),0,1);
  rt_event_send(reg_event,REG_PROTECT_MSK);
  rt_thread_delay_hmsm(0,0,1,0);
  rt_event_recv(reg_event,REG_PROTECT_MSK,RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR,0,&e);
  e = 0;
  while (1)
  {
    voltage = GetVoltage();
	if (e & EVENT_GET_VOLTAGE)
	{
        rt_event_send(reg_event,REG_PROTECT_MSK);
		if (voltage<3.0) 
			reg_output[REG_PROTECT]=0xABE3F9F9;
		else
			reg_output[REG_PROTECT]=REG_Convert(0x63,REG_HexToReg(voltage%10),REG_HexToReg(voltage/10%10),REG_HexToReg(voltage/100%10),0,1);
		rt_thread_delay_hmsm(0,0,1,0);
		WAIT_FOR_RELEASE;
        rt_event_recv(reg_event,REG_PROTECT_MSK,RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR,0,&e);
	}
	
    while ((voltage>3.0)&&(voltage<=108))
    {
      rt_event_send(reg_event,REG_PROTECT_MSK);
		if (voltage<3.0) 
			reg_output[REG_PROTECT]=0xABE3F9F9;
		else
			reg_output[REG_PROTECT]=REG_Convert(0x63,REG_HexToReg(voltage%10),REG_HexToReg(voltage/10%10),REG_HexToReg(voltage/100%10),0,1);
      voltage = GetVoltage();
      rt_thread_delay_hmsm(0,0,0,667);
      rt_event_recv(reg_event,REG_PROTECT_MSK,RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR,0,&e);
      rt_thread_delay_hmsm(0,0,0,333);
    }
   	rt_event_recv(en_event,EVENT_GET_VOLTAGE,RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR,RT_TICK_PER_SECOND*60,&e);
  }
}
