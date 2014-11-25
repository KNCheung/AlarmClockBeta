#ifndef __DEBUG_H_
#define __DEBUG_H_

#include <stm32f10x.h>
#include "stdint.h"

void Debug_Init(void);
#define DebugState (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8)==Bit_SET)

#endif
