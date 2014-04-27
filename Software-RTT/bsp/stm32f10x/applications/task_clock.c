#include <rtthread.h>
#include <board.h>
#include <application.h>

#include <IIC.h>
#include <REG.h>

extern uint32_t reg_output[REG_SCREEN_NUMBER];
extern rt_timer_t update_clock_timer;
extern rt_event_t reg_event,en_event;

uint8_t clock_h,clock_m,clock_s;

void update_timeout(void* parameter)
{
  rt_event_send(en_event,EVENT_CLOCK);
  return;
}

void update_second(void* parameter)
{
  clock_s++;
  clock_s%=60;
  return;
}

void rt_thread_clock_entry(void* parameter)
{
  uint32_t set;
  uint8_t h,m,s;
  rt_uint32_t e,e1;
  IIC_Init();
  update_clock_timer = rt_timer_create("Update", \
                                       update_timeout, \
                                       RT_NULL, \
                                       1, \
                                       RT_TIMER_FLAG_ACTIVATED | RT_TIMER_FLAG_ONE_SHOT | RT_TIMER_FLAG_HARD_TIMER);
  rt_timer_start(update_clock_timer);
  rt_event_send(reg_event,REG_CLOCK_MSK);
  rt_event_send(en_event,EVENT_CLOCK);
  while (1)
  {
    rt_event_recv(en_event,EVENT_CLOCK|EVENT_TEMP_CLOCK,RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,RT_WAITING_FOREVER,&e);
    rt_event_recv(en_event,EVENT_REG,RT_EVENT_FLAG_AND,RT_WAITING_FOREVER,&e1);
    rt_enter_critical();
    h=IIC_Read(DS3231_ADDRESS, DS3231_HOUR);
    m=IIC_Read(DS3231_ADDRESS, DS3231_MINUTE);
    s=IIC_Read(DS3231_ADDRESS, DS3231_SECOND);
    rt_exit_critical();
    clock_h=(h>>4)*10+h%16;
    clock_m=(m>>4)*10+m%16;
    clock_s=(s>>4)*10+s%16;
    set = RT_TICK_PER_SECOND * ((59==clock_s)?1:59-clock_s);
    reg_output[REG_CLOCK]=REG_Convert(REG_HexToReg(m%16),REG_HexToReg(m>>4),REG_HexToReg(h%16),REG_HexToReg(h>>4),1,0);
    rt_timer_control(update_clock_timer,RT_TIMER_CTRL_SET_TIME,&set);
    rt_timer_start(update_clock_timer);
    if (e&EVENT_TEMP_CLOCK)
    {
      reg_output[REG_TEMP_CLOCK]=reg_output[REG_CLOCK];
      rt_event_send(reg_event,REG_TEMP_CLOCK_MSK);
	  rt_thread_delay_hmsm(0,0,1,0);
      WAIT_FOR_RELEASE;
      rt_event_recv(reg_event,REG_TEMP_CLOCK_MSK,RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR,0,&e);
    }
  }
}


