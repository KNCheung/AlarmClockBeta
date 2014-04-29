#ifndef _TASK_ALARM_H_
#define _TASK_ALARM_H_

#include <stdint.h>
#include <stm32f10x.h>
void rt_thread_alarm_entry (void *parameter);
ErrorStatus fnSetAlarmClock(uint8_t hour,uint8_t minute,uint8_t second);

#endif
