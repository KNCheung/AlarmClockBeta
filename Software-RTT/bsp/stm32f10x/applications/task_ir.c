#include <rtthread.h>
#include <board.h>
#include <application.h>

#include <IR.h>
#include <REG.h>
#include <task_reg.h>

extern rt_mq_t ir_mq,key_mq;
extern rt_event_t en_event;

void rt_thread_ir_entry(void* parameter)
{
  uint16_t TS;
  uint8_t state = 0;
  uint32_t IRCode;
  uint8_t RFlag;
  uint8_t t;
  IR_Init();
  IR_Enable();
  while (1)
  {
    rt_mq_recv(ir_mq,&TS,sizeof(uint16_t),RT_WAITING_FOREVER);
    switch (state)
    {
      case 0:
        if ((1236<=TS)&&(TS<=1444))
        {
          state = 1;
          IRCode = 0;
          RFlag = 0;
        }
        else if (RFlag && (((3500<=TS)&&(TS<=4500))||((9350<=TS)&&(TS<=9820))))
          state = 100;
        else
        {
          state = 0;
          RFlag = 0;
        }
        break;
      case  1:case  2:case  3:case  4:case  5:case  6:case  7:case  8:case  9:case 10:
      case 11:case 12:case 13:case 14:case 15:case 16:case 17:case 18:case 19:case 20:
      case 21:case 22:case 23:case 24:case 25:case 26:case 27:case 28:case 29:case 30:
      case 31:case 32:
        if ((80<=TS)&&(TS<=160))
          IRCode*=2;
        else if ((180<=TS)&&(TS<=250))
          IRCode=IRCode*2+1;
        else 
        {
          state = 0;
          break;
        }
        state++;
        if (state==33)
        {
          state=0;
          RFlag =1 ;
          switch ((uint8_t)((IRCode>>8)%256))
          { //DECODER
            // CH-
            case 0xA2: ToggleREG(); break;
            // CH
            case 0x62: rt_event_send(en_event,EVENT_GET_VOLTAGE); break;
            // CH+
            case 0xE2: rt_event_send(en_event,EVENT_TEMP_CLOCK); break;
            // PREV
            case 0x22: rt_event_send(en_event,EVENT_DATE); break;
            // NEXT
            case 0x02: rt_event_send(en_event,EVENT_TEMP); break;
            // PLAY/PAUSE
            case 0xC2: rt_event_send(en_event,EVENT_COUNTER); break;
            // VOL-
            case 0xE0: rt_event_send(en_event,EVENT_POMODORO); break;
            // VOL+
            case 0xA8: rt_event_send(en_event,EVENT_POMODORO_BREAK); break;
            // PLAY/PAUSE
            case 0x90: rt_event_send(en_event,EVENT_POMODORO_REST);break;
            // 0
            case 0x68: t=0; rt_mq_send(key_mq,&t,sizeof(uint8_t)); break;
            // 100+
            case 0x98: t=0x0A; rt_mq_send(key_mq,&t,sizeof(uint8_t)); break;
            // 200+
            case 0xB0: t=0x0B; rt_mq_send(key_mq,&t,sizeof(uint8_t)); break;
            // 1
            case 0x30: t=1; rt_mq_send(key_mq,&t,sizeof(uint8_t)); break;
            // 2
            case 0x18: t=2; rt_mq_send(key_mq,&t,sizeof(uint8_t)); break;
            // 3
            case 0x7A: t=3; rt_mq_send(key_mq,&t,sizeof(uint8_t)); break;
            // 4
            case 0x10: t=4; rt_mq_send(key_mq,&t,sizeof(uint8_t)); break;
            // 5
            case 0x38: t=5; rt_mq_send(key_mq,&t,sizeof(uint8_t)); break;
            // 6
            case 0x5A: t=6; rt_mq_send(key_mq,&t,sizeof(uint8_t)); break;
            // 7
            case 0x42: t=7; rt_mq_send(key_mq,&t,sizeof(uint8_t)); break;
            // 8
            case 0x4A: t=8; rt_mq_send(key_mq,&t,sizeof(uint8_t)); break;
            // 9
            case 0x52: t=9; rt_mq_send(key_mq,&t,sizeof(uint8_t)); break;
            // ERROR
            default: rt_kprintf("INVALID CODE : %x\n",(uint8_t)((IRCode>>8)%256));break;
          }
        }
        break;
      case 100:
        if ((1110<=TS)&&(TS<=1130))
        {
          rt_event_send(en_event,EVENT_PRESS);
          RFlag=1;
        }
        state = 0;
        break;
      default:
        state=0;  
    }
  }
}
