#ifndef __AM2302_H_
#define __AM2302_H_

#include <stm32f10x.h>
#include "stdint.h"

void AM2302_Init(void);
ErrorStatus AM2302_Read(int16_t* humi,int16_t* temp);

#endif
