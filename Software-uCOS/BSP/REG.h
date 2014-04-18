#ifndef __REG_H_
#define __REG_H_

#include "includes.h"

void REG_Init(void);
void REG_Set(uint8_t a,uint8_t b,uint8_t c,uint8_t d,uint8_t comment,uint8_t dot);
void REG_Update(void);
uint8_t REG_HexToReg(uint8_t);

#define REG_PORT GPIOA
#define REG_RCC_CLK RCC_APB2Periph_GPIOA

#define REG_SER GPIO_Pin_2
#define REG_nOE GPIO_Pin_0
#define REG_RCLK GPIO_Pin_1
#define REG_SECLK GPIO_Pin_3

#define REG_On() GPIO_ResetBits(REG_PORT,REG_nOE);
#define REG_Off() GPIO_SetBits(REG_PORT,REG_nOE);

#endif
