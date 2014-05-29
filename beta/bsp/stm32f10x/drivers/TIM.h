#ifndef __TIM_H_
#define __TIM_H_

#include <stdint.h>
void cfgTIM(void);
void TIM2_IRQHandler(void);
void Delay_us(uint16_t);
void fnCfgTIM2(void);

#endif
