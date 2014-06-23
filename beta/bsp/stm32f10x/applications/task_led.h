#ifndef __TASK_LED_H__
#define __TASK_LED_H__

//=====INCLUDE=====
#include <stdint.h>

//=====CONSTANT=====
#define LED_CHANGING	0
#define LED_CHANGED		1
//=====FUNCTIONS=====
void set_led(uint32_t a,uint32_t b,uint32_t t);
void task_led_init(void);

#endif
