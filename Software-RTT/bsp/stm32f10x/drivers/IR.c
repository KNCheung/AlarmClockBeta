#include <stm32f10x.h>
#include <rtthread.h>
#include "IR.h"

static uint16_t LastTS,IRInt;
uint32_t IRCode;

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

extern rt_mq_t ir_mq;
void TIM3_IRQHandler(void)
{  
  uint16_t tmp;
  rt_interrupt_enter();
  if (TIM_GetITStatus(TIM3,TIM_IT_CC1)==SET)
  {
    TIM_ClearITPendingBit(TIM3,TIM_IT_CC1);
    tmp=TIM_GetCounter(TIM3);
    IRInt=tmp>LastTS?tmp-LastTS:tmp+59999-LastTS;
    LastTS=tmp;
    rt_mq_send(ir_mq,&IRInt,sizeof(uint16_t));
  }
  rt_interrupt_leave();
}
