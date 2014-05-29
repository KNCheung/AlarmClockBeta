#include "board.h"

void Delay_us(uint16_t n)
{
  uint16_t t;
  t=TIM_GetCounter(TIM3);
  while ((uint16_t)(TIM_GetCounter(TIM4)-t)<n);
	return;
}

void cfgTIM4()
{
  TIM_TimeBaseInitTypeDef usrTIM;
	
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
  
  usrTIM.TIM_ClockDivision = TIM_CKD_DIV1;
  usrTIM.TIM_CounterMode = TIM_CounterMode_Up;
  usrTIM.TIM_Period = 0xFFFF;
  usrTIM.TIM_Prescaler = SystemCoreClock/1000000-1;
  TIM_TimeBaseInit(TIM4,&usrTIM);
  TIM_Cmd(TIM4,ENABLE);
}


void cfgTIM2(void)
{	
	GPIO_InitTypeDef usrGPIO;
	TIM_TimeBaseInitTypeDef usrTIMBase;
	TIM_OCInitTypeDef usrTIMOC;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	
	usrGPIO.GPIO_Mode = GPIO_Mode_AF_OD;
	usrGPIO.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
	usrGPIO.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA,&usrGPIO);
	
	usrTIMBase.TIM_ClockDivision = TIM_CKD_DIV1;
	usrTIMBase.TIM_CounterMode = TIM_CounterMode_Up;
	usrTIMBase.TIM_Period = 255;
	usrTIMBase.TIM_Prescaler = 0;
	usrTIMBase.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2,&usrTIMBase);
	
	usrTIMOC.TIM_OCMode = TIM_OCMode_PWM1;
	usrTIMOC.TIM_OCIdleState = TIM_OCIdleState_Set;
	usrTIMOC.TIM_OCPolarity = TIM_OCPolarity_Low;
	usrTIMOC.TIM_OutputState = TIM_OutputState_Enable;
	usrTIMOC.TIM_Pulse = 0;
	TIM_OC2Init(TIM2,&usrTIMOC);
	
	TIM_OC2PreloadConfig(TIM2,TIM_OCPreload_Enable);
	TIM_SetCompare2(TIM2,0);
	TIM_OC3Init(TIM2,&usrTIMOC);
	TIM_OC3PreloadConfig(TIM2,TIM_OCPreload_Enable);
	TIM_SetCompare3(TIM2,0);
	TIM_OC4Init(TIM2,&usrTIMOC);
	TIM_OC4PreloadConfig(TIM2,TIM_OCPreload_Enable);
	TIM_SetCompare4(TIM2,0);
	
	TIM_Cmd(TIM2,ENABLE);
}

extern uint32_t SYS_TICK;
uint32_t millis(void)
{
	return (SYS_TICK*10);
}
