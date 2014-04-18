#include "TIM.h"

static uint16_t TIM_cnt=0;
void TIM2_IRQHandler(void)
{
  TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
  TIM_cnt--;
}

void Delay_us(uint32_t n)
{
  TIM_cnt=n;
  TIM_Cmd(TIM2,ENABLE);
  while (TIM_cnt>0);
  TIM_Cmd(TIM2,DISABLE);
}

void cfgTIM()
{
  TIM_TimeBaseInitTypeDef usrTIM;
  NVIC_InitTypeDef usrNVIC;
  
  usrNVIC.NVIC_IRQChannel = TIM2_IRQn;
  usrNVIC.NVIC_IRQChannelCmd = ENABLE;
  usrNVIC.NVIC_IRQChannelPreemptionPriority = 0;
  usrNVIC.NVIC_IRQChannelSubPriority = 0;
  NVIC_Init(&usrNVIC);
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
  
  usrTIM.TIM_ClockDivision = TIM_CKD_DIV1;
  usrTIM.TIM_CounterMode = TIM_CounterMode_Up;
  usrTIM.TIM_Period = 70;
  usrTIM.TIM_Prescaler = 0;
  TIM_TimeBaseInit(TIM2,&usrTIM);
  TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
  TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
  TIM_Cmd(TIM2,DISABLE);
}
