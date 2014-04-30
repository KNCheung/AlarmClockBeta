#ifndef _TASK_ALARM_H_
#define _TASK_ALARM_H_

#include <stdint.h>
#include <stm32f10x.h>
void rt_thread_alarm_entry (void *parameter);
ErrorStatus fnSetAlarmClock(uint8_t hour,uint8_t minute,uint8_t second);
void AlarmerInit(void);

#define SetAlarm(x) {	if ((x>100)&&(x<1024)) \
						{TIM_CtrlPWMOutputs(TIM1,ENABLE);TIM_Cmd(TIM1,ENABLE);TIM_SetCompare1(TIM1,x);} \
						else {TIM_CtrlPWMOutputs(TIM1,DISABLE);TIM_Cmd(TIM1,DISABLE);}}

#endif
