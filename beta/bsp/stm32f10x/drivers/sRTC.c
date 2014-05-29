#include <board.h>
#include <rtthread.h>

#define rt_tick rt_tick_get()

void sRTC_DelayMS(uint32_t n);
void sRTC_DelayUS(uint32_t n);

uint32_t sRTC_GetUS(void);
uint16_t sRTC_GetMS(void);

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

