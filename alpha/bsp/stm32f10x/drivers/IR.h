#ifndef __IR_H_
#define __IR_H_

void IR_Init(void);
void IR_Disable(void);
void IR_Enable(void);

void TIM3_IRQHandler(void);
void IR_Hook(void);

#endif
