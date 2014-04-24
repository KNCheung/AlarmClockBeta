#ifndef __TIM_H_
#define __TIM_H_

#include <stdint.h>
void cfgTIM(void);
void Delay_us(uint32_t);
void TIM4_IRQHandler(void);

#endif
