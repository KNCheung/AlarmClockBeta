#ifndef __TIM_H_
#define __TIM_H_


#define LED_R(x) TIM_SetCompare2(TIM2,(x))
#define LED_G(x) TIM_SetCompare3(TIM2,(x))
#define LED_B(x) TIM_SetCompare4(TIM2,(x))
#define LED_RGB(x,y,z) do{LED_R(x);LED_G(y);LED_B(z);}while(0)

#include <stdint.h>
void cfgTIM(void);
void TIM2_IRQHandler(void);
void Delay_us(uint16_t);
void fnCfgTIM2(void);

#endif
