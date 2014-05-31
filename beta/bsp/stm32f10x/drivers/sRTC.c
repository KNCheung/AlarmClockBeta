#include <board.h>
#include <rtthread.h>
#include <stdlib.h>
#define rt_tick rt_tick_get()

extern uint8_t rtc_Y,rtc_M,rtc_D,rtc_h,rtc_m,rtc_s;

void sRTC_DelayMS(uint32_t n);
void sRTC_DelayUS(uint32_t n);

uint32_t sRTC_GetUS(void);
uint16_t sRTC_GetMS(void);
uint32_t sRTC_GetTS(void);
void sRTC_GetRTC(uint32_t ts,uint8_t* Y,uint8_t* M,uint8_t* D,uint8_t* h,uint8_t* m,uint8_t* s);

uint32_t sRTC_GetTS(void)
{
	uint32_t tmp;
	tmp=0;
	switch (rtc_M)
	{
		case 12:tmp+=30;
		case 11:tmp+=31;
		case 10:tmp+=30;
		case 9:tmp+=31;
		case 8:tmp+=31;
		case 7:tmp+=30;
		case 6:tmp+=31;
		case 5:tmp+=30;
		case 4:tmp+=31;
		case 3:tmp+=28;
		case 2:tmp+=31;
		case 1:tmp+=00;
	}
	tmp+=rtc_Y/4+1;
	if ((rtc_Y%4)&&(rtc_M<=2))
		tmp--;
	tmp+=365*rtc_Y+rtc_D;
	return (rtc_s+rtc_m*60+rtc_h*3600+tmp*86400);
}


void sRTC_GetRTC(uint32_t ts,uint8_t* Y,uint8_t* M,uint8_t* D,uint8_t* h,uint8_t* ret_m,uint8_t* s)
{
	uint8_t y,m;
	if (s!=NULL) *s=ts%60;
	ts/=60;
	if (ret_m!=NULL) *ret_m=ts%60; 
	ts/=60;
	if (h!=NULL) *h=ts%24;
	ts/=24;
	y=0;
	while (ts>=(365+((y%4==0)?1:0)))
	{
		ts-=(365+((y%4==0)?1:0));
		y++;
	}
	if (Y!=NULL) *Y=y;
	m=1;
	if (ts>31) {m++;ts-=31;			//Jan
	if (ts>(((y%4==0)?1:0)+28))
	{m++;ts-=((y%4==0)?1:0)+28;	//Feb
	if (ts>31) {m++;ts-=31;			//Mar
	if (ts>30) {m++;ts-=30;			//Apr
	if (ts>31) {m++;ts-=31;			//May
	if (ts>30) {m++;ts-=30;			//Jun
	if (ts>31) {m++;ts-=31;			//Jul
	if (ts>31) {m++;ts-=31;			//Aug
	if (ts>30) {m++;ts-=30;			//Sep
	if (ts>31) {m++;ts-=31;			//Oct
	if (ts>30) {m++;ts-=30;			//Nov
	if (ts>31) {m++;ts-=31;			//Dec
	}}}}}}}}}}}}
	if (M!=NULL) *M=m;
	if (D!=NULL) *D=ts;
	return;
}

uint32_t sRTC_GetUS(void)
{
	volatile uint32_t ret,t;
	t=SystemCoreClock/1000000;
	ret = (uint32_t)((uint32_t)(rt_tick*SysTick->LOAD/t)+ (uint32_t)(SysTick->VAL)/t);
	return (uint32_t)ret%1000000;
}

uint16_t sRTC_GetMS(void)
{
	volatile uint32_t ret,t;
	t=SystemCoreClock/1000;
	ret = (uint32_t)((uint32_t)(rt_tick*SysTick->LOAD/t)+ (uint32_t)(SysTick->VAL/t));
	return (uint16_t)ret;
}

void sRTC_DelayUS(uint32_t n)
{
	uint32_t ts,SysTS,i;
	ts = SysTick->VAL;
	SysTS=rt_tick;
	n *= (SystemCoreClock/1000000);
	i = n/SysTick->LOAD;
	n%= SysTick->LOAD;
	while (rt_tick-SysTS<i);
	while ((SysTick->VAL-ts)%SysTick->LOAD<n);
}

void sRTC_DelayMS(uint32_t n)
{
	volatile uint16_t t;
	t = sRTC_GetMS();
	while ((uint16_t)((uint16_t)sRTC_GetMS()-(uint16_t)t)<n);
}

