#ifndef __TIM_H_
#define __TIM_H_

#include <stdint.h>
void cfgTIM4(void);
void cfgTIM2(void);
void TIM2_IRQHandler(void);
void Delay_us(uint16_t);
uint32_t millis(void);

#endif
