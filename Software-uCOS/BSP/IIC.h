#ifndef __IIC_H_
#define __IIC_H_

#include "includes.h"

void IIC_Init(void);
uint8_t IIC_Read(uint8_t Address,uint8_t Register);
void IIC_Write(uint8_t Address,uint8_t Register,uint8_t Data);

#endif
