#ifndef __REG_H_
#define __REG_H_

#include <rtthread.h>
#include <stm32f10x.h>

void REG_Init(void);
uint32_t REG_Convert(rt_uint8_t a,rt_uint8_t b,rt_uint8_t c,rt_uint8_t d,rt_uint8_t comment,rt_uint8_t dot);
void REG_Set(uint32_t);
void REG_Update(void);
rt_uint8_t REG_HexToReg(rt_uint8_t);

#define REG_PORT GPIOA
#define REG_RCC_CLK RCC_APB2Periph_GPIOA

#define REG_SER GPIO_Pin_4
#define REG_nOE GPIO_Pin_2
#define REG_RCLK GPIO_Pin_1
#define REG_SECLK GPIO_Pin_3

#define REG_On() GPIO_ResetBits(REG_PORT,REG_nOE)
#define REG_Off() GPIO_SetBits(REG_PORT,REG_nOE)
#define REG_Toggle() GPIO_WriteBit(REG_PORT,REG_nOE,!GPIO_ReadOutputDataBit(REG_PORT,REG_nOE))
#define REG_State() (!GPIO_ReadOutputDataBit(REG_PORT,REG_nOE))

#endif
