#include "IR.h"

static uint16_t LastTS,IRInt;
unsigned long IRCode;

void IR_Init(void)
{
  GPIO_InitTypeDef usrGPIO;
  NVIC_InitTypeDef usrNVIC;
  TIM_ICInitTypeDef usrTIMIC;
  TIM_TimeBaseInitTypeDef usrTIMBase;
  
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
  
  usrNVIC.NVIC_IRQChannel = TIM3_IRQn;
  usrNVIC.NVIC_IRQChannelPreemptionPriority = 1;
  usrNVIC.NVIC_IRQChannelSubPriority = 1;
  usrNVIC.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&usrNVIC);
  
  TIM_DeInit(TIM3);
  usrTIMBase.TIM_ClockDivision = TIM_CKD_DIV1 ;
  usrTIMBase.TIM_CounterMode = TIM_CounterMode_Up;
  usrTIMBase.TIM_Period = 59999;
  usrTIMBase.TIM_Prescaler = 719;
  TIM_TimeBaseInit(TIM3,&usrTIMBase);
  
  usrTIMIC.TIM_Channel = TIM_Channel_1;
  usrTIMIC.TIM_ICFilter = 0;
  usrTIMIC.TIM_ICPolarity = TIM_ICPolarity_Falling;
  usrTIMIC.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  usrTIMIC.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM_ICInit(TIM3,&usrTIMIC);
  
  usrGPIO.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  usrGPIO.GPIO_Pin = GPIO_Pin_6;
  usrGPIO.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA,&usrGPIO);
}

void IR_Enable(void)
{
  TIM_ITConfig(TIM3,TIM_IT_CC1,ENABLE);
  TIM_SetCounter(TIM3,0);
  TIM_Cmd(TIM3,ENABLE);
  TIM_ClearITPendingBit(TIM3,TIM_IT_CC1);
}

void IR_Disable(void)
{
  TIM_Cmd(TIM3,DISABLE);
  TIM_ITConfig(TIM3,TIM_IT_CC1,DISABLE);
}


void IR_Hook(void)
{
  static uint8_t flag=0;
  static uint8_t state=0;
  switch (state)
  {
    case 0:
      if ((1336<=IRInt)&&(IRInt<=1344))
      {
        state = 1;
        IRCode = 0;
        flag = 0;
      }
      else if (flag && (((4000<=IRInt)&&(IRInt<=4050))||((9550<=IRInt)&&(IRInt<=9620))))
        state = 100;
      else
        state = 0;
      break;
    case  1:
    case  2:
    case  3:
    case  4:
    case  5:
    case  6:
    case  7:
    case  8:
    case  9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
    case 27:
    case 28:
    case 29:
    case 30:
    case 31:
    case 32:
      if ((100<=IRInt)&&(IRInt<=120))
        IRCode*=2;
      else if ((210<=IRInt)&&(IRInt<=235))
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
        flag =1 ;
        IR_CodeReceived();
      }
      break;
    case 100:
      if ((1110<=IRInt)&&(IRInt<=1130))
      {
        state=0;
        IR_CodeRepeated();
      }
    default:
      state=0;
  }
}

extern uint8_t FlagIR;
void IR_CodeReceived(void)
{
  FlagIR = 1;
}

void IR_CodeRepeated(void)
{
}

void TIM3_IRQHandler(void)
{  
  uint16_t tmp;
  if (TIM_GetITStatus(TIM3,TIM_IT_CC1)==SET)
  {
    TIM_ClearITPendingBit(TIM3,TIM_IT_CC1);
    tmp=TIM_GetCounter(TIM3);
    IRInt=tmp>LastTS?tmp-LastTS:tmp+59999-LastTS;
    LastTS=tmp;
    IR_Hook();
  }
}
