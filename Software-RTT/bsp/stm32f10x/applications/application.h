#ifndef _APPLICATION_H_
#define _APPLICATION_H_

//Define Maskbit of en_event
#define EVENT_REG (1<<0)
#define EVENT_CLOCK (1<<2)
#define EVENT_TEMP_CLOCK (1<<8)
#define EVENT_TEMP (1<<3)
#define EVENT_DATE (1<<4)
#define EVENT_POMODORO (1<<5)
#define EVENT_POMODORO_BREAK (1<<6)
#define EVENT_POMODORO_REST (1<<7)
#define EVENT_PRESS (1<<1)

//Define Setting
#define SETTING_LENGTH 16
#define SETTING_BASE 0x0000
#define POMODORO_TIME 1
#define POMODORO_BREAK_TIME 2
#define POMODORO_REST_TIME 3

//Define REG Screen
#define REG_SCREEN_NUMBER 	10
#define REG_TEMP_CLOCK		0
#define REG_DATE 			1
#define REG_TEMP 			2
#define REG_POMODORO		3
#define REG_COUNTER			4
#define REG_CLOCK 			5

//Define Macro
#define WAIT_FOR_RELEASE while(rt_event_recv(en_event,EVENT_PRESS,RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,RT_TICK_PER_SECOND/8,&e)==RT_EOK)

#include <stdint.h>

void reboot(void);
void fnDebug(uint8_t a,uint8_t b,uint8_t c);
void Print_Setting(void);
void Setting_Write(void);
void Setting_Read(void);
void pomodoro_set(uint8_t a,uint8_t b,uint8_t c);
void time(void);
void lp(void);


#endif 
