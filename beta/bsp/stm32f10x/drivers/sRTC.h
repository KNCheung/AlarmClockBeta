#ifndef _SRTC_H_
#define _SRTC_H_

#include <board.h>

void sRTC_DelayMS(uint32_t n);
void sRTC_DelayUS(uint32_t n);

uint32_t sRTC_GetUS(void);
uint16_t sRTC_GetMS(void);

uint32_t sRTC_GetTS(void);
void sRTC_GetRTC(uint32_t ts,uint8_t* Y,uint8_t* M,uint8_t* D,uint8_t* h,uint8_t* m,uint8_t* s);

#endif
