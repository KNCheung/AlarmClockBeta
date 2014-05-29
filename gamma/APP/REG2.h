#ifndef _REG2_H_
#define _REG2_H_

#include "board.h"
#include <stdarg.h>

#define SHIFT_DELAY	{\
	}while(0)

#define MSBFIRST 1
#define LSBFIRST 0

#define REG1		0x01
#define REG2		0x02
#define REG3		0x04
#define REG4		0x08
#define REG5		0x10
#define REG6		0x20
#define REG7		0x40
#define REG8		0x80
#define REGA		0xFF
	
#define REG_NONE	0x00
#define REG_X1		0x01
#define REG_X2		0x02
#define REG_X3		0x03
#define REG_X4		0x04
#define REG_X5		0x05
#define REG_X6		0x06
#define REG_X7		0x07
#define REG_X8		0x08
#define REG_Dec		0x09
#define REG_Int		0x0a
#define REG_Sca		0x0b
#define REG_Shut	0x0c
#define REG_Test	0x0f

void REG2_Init(uint8_t n);
void ShiftOut(GPIO_TypeDef *GPIOx, uint16_t SDA,uint16_t SCL, uint8_t order, uint8_t data);
void PushREG(uint8_t n, uint8_t REG, ...);
void PushBitMap(uint8_t n, uint8_t *BitMap);

#endif
