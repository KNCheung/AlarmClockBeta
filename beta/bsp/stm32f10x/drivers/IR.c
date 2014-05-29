#include <board.h>
#include <rtthread.h>

static uint16_t LastTS,IRInt;
unsigned long IRCode;

#define IR_PORT		GPIOB
#define IR_PIN		GPIO_Pin_13
#define IR_PORTS	GPIO_PortSourceGPIOB
#define IR_PINS		GPIO_PinSource13

#define IR_GETTIME		TIM_GetCounter(TIM4)

void IR_Hook(void);

void IR_CodeReceived(void);
void IR_CodeRepeated(void);
void IR_CodeReceivedFail(void);

void IR_Init(void)
{
	GPIO_InitTypeDef usrGPIO;
	NVIC_InitTypeDef usrNVIC;  
	EXTI_InitTypeDef usrEXTI;

	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
	usrNVIC.NVIC_IRQChannel = EXTI15_10_IRQn;
	usrNVIC.NVIC_IRQChannelPreemptionPriority = 1;
	usrNVIC.NVIC_IRQChannelSubPriority = 1;
	usrNVIC.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&usrNVIC);
	
	usrEXTI.EXTI_Line = EXTI_Line13;
	usrEXTI.EXTI_Mode = EXTI_Mode_Interrupt;
	usrEXTI.EXTI_LineCmd = ENABLE;
	usrEXTI.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_Init(&usrEXTI);
  
	usrGPIO.GPIO_Mode = GPIO_Mode_IPD;
	usrGPIO.GPIO_Pin = IR_PIN;
	usrGPIO.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(IR_PORT,&usrGPIO);
	GPIO_EXTILineConfig(IR_PORTS,IR_PINS);
	
	LastTS=IR_GETTIME;
}

void EXTI15_10_IRQHandler(void)
{  
  uint16_t tmp;
  rt_interrupt_enter();
  if (EXTI_GetITStatus(EXTI_Line13)==SET)
  {
    tmp=IR_GETTIME;
    IRInt=(uint16_t)(tmp-LastTS);
    LastTS=tmp;
    IR_Hook();
    EXTI_ClearITPendingBit (EXTI_Line13);
  }
  rt_interrupt_leave();
}

void IR_Hook(void)
{
  static uint8_t flag=0;
  static uint8_t state=0;
  switch (state)
  {
    case 0:
      if ((13360<=IRInt)&&(IRInt<=13440))
      {
        state = 1;
        IRCode = 0;
        flag = 0;
      }
      else if (flag && ((30000<=IRInt)&&(IRInt<=31000)))
        state = 100;
      else
        state = 0;
      break;
    case  1:    case  2:    case  3:    case  4:    case  5:
    case  6:    case  7:    case  8:    case  9:    case 10:
    case 11:    case 12:    case 13:    case 14:    case 15:
    case 16:    case 17:    case 18:    case 19:    case 20:
    case 21:    case 22:    case 23:    case 24:    case 25:
    case 26:    case 27:    case 28:    case 29:    case 30:
    case 31:    case 32:
      if ((1000<=IRInt)&&(IRInt<=1200))
        IRCode=IRCode*2+1;
      else if ((2100<=IRInt)&&(IRInt<=2350))
        IRCode*=2;
      else 
      {
        state = 0;
        break;
      }
      state++;
      if (state==33)
      {
		state=0;
		if (((((IRCode>>8)&0x00FF00FF)|((IRCode<<8)&0xFF00FF00))^IRCode)==0xFFFFFFFF)
		{
			flag =1;
			IR_CodeReceived();
		}else{
			IR_CodeReceivedFail();
		}
      }
      break;
    case 100:
      if ((11100<=IRInt)&&(IRInt<=11300))
      {
        state=0;
        IR_CodeRepeated();
      }
	default:
		state=0;
  }
}

extern rt_mq_t mq_ir;

void IR_CodeReceived(void)
{
	rt_mq_send(mq_ir,&IRCode,sizeof(uint32_t));
}

void IR_CodeRepeated(void)
{
	IRCode=0xFFFFFFFF;
	rt_mq_send(mq_ir,&IRCode,sizeof(uint32_t));
}

void IR_CodeReceivedFail(void)
{
}
