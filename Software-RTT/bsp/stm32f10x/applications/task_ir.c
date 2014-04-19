#include <rtthread.h>
#include <board.h>
#include <application.h>

#include <IR.h>
#include <REG.h>
#include <task_reg.h>

extern rt_mq_t ir_mq;
extern rt_event_t en_event;

void rt_thread_ir_entry(void* parameter)
{
  uint16_t TS;
  uint8_t state = 0;
  uint32_t IRCode;
  uint8_t RFlag;
  IR_Init();
  IR_Enable();
  ir_mq = rt_mq_create("IR",sizeof(uint16_t),64,RT_IPC_FLAG_FIFO);
  while (1)
  {
    rt_mq_recv(ir_mq,&TS,sizeof(uint16_t),RT_WAITING_FOREVER);
    switch (state)
    {
      case 0:
        if ((1336<=TS)&&(TS<=1344))
        {
          state = 1;
          IRCode = 0;
          RFlag = 0;
        }
        else if (RFlag && (((4000<=TS)&&(TS<=4050))||((9550<=TS)&&(TS<=9620))))
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
        if ((100<=TS)&&(TS<=120))
          IRCode*=2;
        else if ((210<=TS)&&(TS<=235))
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
          { //DECODE
            // CH-
            case 0xA2: ToggleREG(); break;
            // CH
            case 0x62: break;
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
            case 0x68:  break;
            // 100+
            case 0x98: break;
            // 200+
            case 0xB0: break;
            // 1
            case 0x30: break;
            // 2
            case 0x18: break;
            // 3
            case 0x7A: break;
            // 4
            case 0x10: break;
            // 5
            case 0x38: break;
            // 6
            case 0x5A: break;
            // 7
            case 0x42: break;
            // 8
            case 0x4A: break;
            // 9
            case 0x52: break;
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
