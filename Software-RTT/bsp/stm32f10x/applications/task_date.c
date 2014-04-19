#include <rtthread.h>
#include <board.h>
#include <application.h>

#include <IIC.h>
#include <REG.h>

extern rt_event_t en_event,reg_event;
extern uint32_t reg_output[REG_SCREEN_NUMBER];

void rt_thread_date_entry(void* parameter)
{
  rt_uint32_t e;
  uint8_t m,d;
  IIC_Init();
  while(1){
    rt_event_recv(en_event,EVENT_DATE,RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR,RT_WAITING_FOREVER,&e);

    m = IIC_Read(DS3231_ADDRESS, DS3231_MONTH);
    d = IIC_Read(DS3231_ADDRESS, DS3231_DAY);
    reg_output[REG_DATE]=REG_Convert(REG_HexToReg(d%16),REG_HexToReg(d/16),REG_HexToReg(m%16),REG_HexToReg((m/16)&0x7),1,1);
    
    rt_event_send(reg_event,1<<REG_DATE);
    WAIT_FOR_RELEASE;
    rt_event_recv(reg_event,(1<<REG_DATE),RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR,0,&e);
  }
}
