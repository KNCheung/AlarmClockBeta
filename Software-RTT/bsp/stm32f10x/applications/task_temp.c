#include <rtthread.h>
#include <board.h>
#include <application.h>

#include <REG.h>
#include <DS18B20.h>
#include <TIM.h>

extern uint32_t reg_output[REG_SCREEN_NUMBER]; 
extern rt_event_t en_event,reg_event;

void rt_thread_temp_entry(void* parameter)
{
  rt_uint32_t e;
  uint16_t temp;
  OneWire_Init();
  DS18B20_StartConvTemp();
  rt_thread_delay(RT_TICK_PER_SECOND/2);
  while (1)
  {
    rt_event_recv(en_event,EVENT_TEMP,RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR,RT_WAITING_FOREVER,&e);

    rt_event_send(reg_event,REG_TEMP_MSK);
    reg_output[REG_TEMP]=0xBFBFBFBF;
    
    rt_enter_critical();
    DS18B20_StartConvTemp();
    rt_exit_critical();
    
    rt_thread_delay(RT_TICK_PER_SECOND/2);
    
    rt_enter_critical();
    temp = DS18B20_ReadTemp()*10;
    rt_exit_critical();
    reg_output[REG_TEMP]=REG_Convert(0x27,REG_HexToReg(temp%10),REG_HexToReg(temp/10%10),REG_HexToReg(temp/100),0,1);
    
    WAIT_FOR_RELEASE;
    rt_event_recv(reg_event,REG_TEMP_MSK,RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR,0,&e);
    
  }
}
