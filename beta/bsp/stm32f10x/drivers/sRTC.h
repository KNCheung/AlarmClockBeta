#ifndef _SRTC_H_
#define _SRTC_H_

#include <board.h>

void sRTC_DelayMS(uint32_t n);
void sRTC_DelayUS(uint32_t n);

uint32_t sRTC_GetUS(void);
uint16_t sRTC_GetMS(void);

#endif
