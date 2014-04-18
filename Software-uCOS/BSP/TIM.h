#ifndef __TIM_H_
#define __TIM_H_

#include "includes.h"

void cfgTIM(void);
void TIM2_IRQHandler(void);
void Delay_us(uint32_t);

#endif
