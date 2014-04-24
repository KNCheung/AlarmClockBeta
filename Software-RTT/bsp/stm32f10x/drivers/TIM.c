#include <stm32f10x.h>
#include <rtthread.h>

static uint16_t TIM_cnt=0;
void TIM4_IRQHandler(void)
{
  TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
  TIM_cnt--;
}

void Delay_us(uint32_t n)
{
  TIM_cnt=n/2;
  TIM_Cmd(TIM4,ENABLE);
  while (TIM_cnt>0);
  TIM_Cmd(TIM4,DISABLE);
}

void cfgTIM()
{
  TIM_TimeBaseInitTypeDef usrTIM;
  NVIC_InitTypeDef usrNVIC;
  
  usrNVIC.NVIC_IRQChannel = TIM4_IRQn;
  usrNVIC.NVIC_IRQChannelCmd = ENABLE;
  usrNVIC.NVIC_IRQChannelPreemptionPriority = 0;
  usrNVIC.NVIC_IRQChannelSubPriority = 0;
  NVIC_Init(&usrNVIC);
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
  
  usrTIM.TIM_ClockDivision = TIM_CKD_DIV1;
  usrTIM.TIM_CounterMode = TIM_CounterMode_Up;
  usrTIM.TIM_Period = 71;
  usrTIM.TIM_Prescaler = 0;
  TIM_TimeBaseInit(TIM4,&usrTIM);
  TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
  TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);
  TIM_Cmd(TIM4,DISABLE);
}
