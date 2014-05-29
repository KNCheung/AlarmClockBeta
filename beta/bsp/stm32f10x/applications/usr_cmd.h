#ifndef __USR_CMD_H_
#define __USR_CMD_H_

#include "stdint.h"
void reboot(void);
void time(void);
void StandBy(void);
void Set_Time(uint8_t Y,uint8_t M,uint8_t D,uint8_t w, uint8_t h,uint8_t m, uint8_t s);


#endif
