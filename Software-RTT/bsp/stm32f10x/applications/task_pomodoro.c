#include <rtthread.h>
#include <board.h>
#include <application.h>

#include <REG.h>

extern rt_event_t en_event,reg_event;
extern uint8_t Setting[SETTING_LENGTH];
extern uint32_t reg_output[REG_SCREEN_NUMBER];
extern uint8_t clock_m,clock_s;
uint16_t clock_time,des_time;

void rt_thread_pomodoro_entry(void* parameter)
{
  rt_uint32_t e;
  uint8_t cnt = 0;
  uint8_t state = 0;
  while (1)
  {
    rt_event_recv(en_event,EVENT_POMODORO,RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR,RT_WAITING_FOREVER,&e);
    rt_event_send(reg_event,REG_POMODORO_MSK);
    state = 0;
    do 
    {
      switch (state)
      {
        case 0: //Initialization for Working
          rt_event_recv(en_event,EVENT_POMODORO_BREAK|EVENT_POMODORO_REST,RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,0,&e);
          des_time = (((Setting[POMODORO_TIME]+clock_m)%60)*60+clock_s)%3600;
          state++;
        case 1: //Working
          cnt = ((des_time+3600-clock_m*60-clock_s)%3600)/60+((clock_s!=(des_time%60))?1:0);
          reg_output[REG_POMODORO]=REG_Convert(REG_HexToReg(22),REG_HexToReg(22),REG_HexToReg(cnt%10),REG_HexToReg(cnt/10),1,1);
          if (cnt==0)
          {
            state++;
            rt_event_recv(en_event, EVENT_POMODORO|EVENT_POMODORO_BREAK|EVENT_POMODORO_REST,RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR,0,&e);
          }
          else
            break;
        case 2: //Waiting for command & Blinking
          if (rt_event_recv(en_event, EVENT_POMODORO|EVENT_POMODORO_BREAK|EVENT_POMODORO_REST,RT_EVENT_FLAG_OR,RT_TICK_PER_SECOND*4,&e)==RT_EOK)
          {
            state++;
            break;
          }
          reg_output[REG_POMODORO]=REG_Convert(REG_HexToReg(22),REG_HexToReg(22),REG_HexToReg(22),REG_HexToReg(22),1,1);
          if (rt_event_recv(en_event, EVENT_POMODORO|EVENT_POMODORO_BREAK|EVENT_POMODORO_REST,RT_EVENT_FLAG_OR,RT_TICK_PER_SECOND*1,&e)==RT_EOK)
          {
            state++;
            break;
          }
          reg_output[REG_POMODORO]=REG_Convert(REG_HexToReg(22),REG_HexToReg(22),REG_HexToReg(0),REG_HexToReg(0),1,1);
          break;
        case 3: //Check commands & Initialization for Break or Rest
          rt_event_recv(en_event,EVENT_POMODORO_BREAK|EVENT_POMODORO_REST,RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,0,&e);
          if (e & EVENT_POMODORO) break;
          else if (e & EVENT_POMODORO_BREAK)          
            des_time = (((Setting[POMODORO_BREAK_TIME]+clock_m)%60)*60+clock_s)%3600;
          else 
            des_time = (((Setting[POMODORO_REST_TIME]+clock_m)%60)*60+clock_s)%3600;
          state++;
        case 4: //Break or Rest
          cnt = ((des_time+3600-clock_m*60-clock_s)%3600)/60+((clock_s!=(des_time%60))?1:0);
          reg_output[REG_POMODORO]=REG_Convert(REG_HexToReg(cnt%10),REG_HexToReg(cnt/10),REG_HexToReg(22),REG_HexToReg(22),1,1);
          if (cnt==0)
          {
            state++;
            rt_event_recv(en_event, EVENT_POMODORO|EVENT_POMODORO_BREAK|EVENT_POMODORO_REST,RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR,0,&e);
          }
          else 
            break;
        case 5: //Waiting for command & Blinking
          if (rt_event_recv(en_event, EVENT_POMODORO|EVENT_POMODORO_BREAK|EVENT_POMODORO_REST,RT_EVENT_FLAG_OR,RT_TICK_PER_SECOND*1,&e)==RT_EOK)
          {
            state++;
            break;
          }
          reg_output[REG_POMODORO]=REG_Convert(REG_HexToReg(22),REG_HexToReg(22),REG_HexToReg(22),REG_HexToReg(22),1,1);
          if (rt_event_recv(en_event, EVENT_POMODORO|EVENT_POMODORO_BREAK|EVENT_POMODORO_REST,RT_EVENT_FLAG_OR,RT_TICK_PER_SECOND*1,&e)==RT_EOK)
          {
            state++;
            break;
          }
          reg_output[REG_POMODORO]=REG_Convert(REG_HexToReg(0),REG_HexToReg(0),REG_HexToReg(22),REG_HexToReg(22),1,1);
          break;
        case 6: //Check Commands
          if (e & EVENT_POMODORO) break;
          else state = 0;
        default: 
          break;
      }
    }while(rt_event_recv(en_event,EVENT_POMODORO,RT_EVENT_FLAG_AND,RT_TICK_PER_SECOND/2,&e) != RT_EOK);
    rt_event_recv(en_event, EVENT_POMODORO|EVENT_POMODORO_BREAK|EVENT_POMODORO_REST,RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR,0,&e);
    rt_event_recv(reg_event, REG_POMODORO_MSK, RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR ,0 ,&e);
  }
}
