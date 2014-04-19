#include <rtthread.h>
#include <board.h>
#include <application.h>

#include <REG.h>
#include <IIC.h>

extern uint32_t reg_output[REG_SCREEN_NUMBER]; 
extern rt_event_t en_event,reg_event;

void rt_thread_temp_entry(void* parameter)
{
  rt_uint32_t e;
  uint8_t temp;
  IIC_Init();
  while (1)
  {
    rt_event_recv(en_event,EVENT_TEMP,RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR,RT_WAITING_FOREVER,&e);

    temp = IIC_Read(DS3231_ADDRESS, DS3231_TEMP_MSB);
    reg_output[REG_TEMP]=REG_Convert(0x27,REG_HexToReg(temp%10),REG_HexToReg(temp/10),REG_HexToReg(0xff),1,1);
    
    rt_event_send(reg_event,REG_TEMP_MSK);
    WAIT_FOR_RELEASE;
    rt_event_recv(reg_event,REG_TEMP_MSK,RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR,0,&e);
  }
}
