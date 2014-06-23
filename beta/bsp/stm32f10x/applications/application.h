#ifndef _APPLICATION_H_
#define _APPLICATION_H_

//Define Maskbit of en_event
#define EVENT_REG 		    		0x00000001
#define EVENT_PRESS 	    		0x00000002
#define EVENT_CLOCK 	    		0x00000004
#define EVENT_TEMP 		    		0x00000008
#define EVENT_DATE 		    		0x00000010
#define EVENT_POMODORO 		   		0x00000020
#define EVENT_POMODORO_BREAK 		0x00000040
#define EVENT_POMODORO_REST 		0x00000080
#define EVENT_TEMP_CLOCK 	 	  	0x00000100
#define EVENT_COUNTER		    	0x00000200
#define EVENT_GET_VOLTAGE			0x00000400

//Define Setting
#define SETTING_LENGTH 			64
#define SETTING_BASE 			0x0000
#define POMODORO_TIME 			0x0001
#define POMODORO_BREAK_TIME 	0x0002
#define POMODORO_REST_TIME 		0x0003
#define ALARM					0x0004

//Define REG Screen
#define REG_SCREEN_NUMBER 	32
#define REG_PROTECT			0
#define REG_TEMP_CLOCK		1
#define REG_ALARM			2
#define REG_DATE 			3
#define REG_TEMP 			4
#define REG_POMODORO		5
#define REG_COUNTER			6
#define REG_CLOCK 			7
#define REG_NOTHING			31

#define REG_PROTECT_MSK 	0x00000001
#define REG_TEMP_CLOCK_MSK	0x00000002
#define REG_ALARM_MSK		0x00000004
#define REG_DATE_MSK		0x00000008
#define REG_TEMP_MSK		0x00000010
#define REG_POMODORO_MSK	0x00000020
#define REG_COUNTER_MSK		0x00000040
#define REG_CLOCK_MSK		0x00000080
#define REG_NOTHING_MSK     0x80000000

#define LED_BLACK			0x00000001
#define LED_RED				0x00000002
#define LED_GREEN			0x00000004
#define LED_BLUE			0x00000008
#define LED_PURPLE			0x00000010
#define LED_CYAN			0x00000020
#define LED_YELLOW			0x00000040
#define LED_WHITE			0x00000080

//Define Macro
#define WAIT_FOR_RELEASE while(rt_event_recv(en_event,EVENT_PRESS,RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,RT_TICK_PER_SECOND/8,&e)==RT_EOK)

//Define Priority
#define PRIO_PROTECT		1
#define PRIO_UI				2
#define PRIO_IR				2
#define PRIO_ALARM			3
#define PRIO_COUNTER		4
#define PRIO_POMODORO		5
#define PRIO_TEMP			6
#define PRIO_DATE			7
#define PRIO_CLOCK			RT_THREAD_PRIORITY_MAX-5
#define PRIO_REG			RT_THREAD_PRIORITY_MAX-3
#define PRIO_ANIMATION		RT_THREAD_PRIORITY_MAX-3
#define PRIO_LED			RT_THREAD_PRIORITY_MAX-3
#define PRIO_DOGE			RT_THREAD_PRIORITY_MAX-1

#define F_ANI_TIME			0x00000001
#define F_ANI_DATE			0x00000002
#define F_ANI_TEMP			0x00000004
#define F_ANI_PREV			0x00000008
#define F_ANI_NEXT			0x00000010
#define F_ANI_CHOOSE		0x00000020

#define F_EN_COUNTER		0x00000001
#define F_EN_POMODORO		0x00000002

#define F_KEY_11			0x00000001
#define F_KEY_12			0x00000002
#define F_KEY_13			0x00000004
#define F_KEY_21			0x00000010
#define F_KEY_22			0x00000020
#define F_KEY_23			0x00000040
#define F_KEY_31			0x00000100
#define F_KEY_32			0x00000200
#define F_KEY_33			0x00000400
#define F_KEY_41			0x00001000
#define F_KEY_42			0x00002000
#define F_KEY_43			0x00004000
#define F_KEY_51			0x00010000
#define F_KEY_52			0x00020000
#define F_KEY_53			0x00040000
#define F_KEY_61			0x00100000
#define F_KEY_62			0x00200000
#define F_KEY_63			0x00400000
#define F_KEY_71			0x01000000
#define F_KEY_72			0x02000000
#define F_KEY_73			0x04000000

#include <task_clock.h>
#include <task_animation.h>
#include <task_ui.h>
#include <task_pomodoro.h>
#include <task_counter.h>
#include <task_temp.h>
#include <task_doge.h>
#include <task_led.h>

#include <usr_cmd.h>

#endif 
