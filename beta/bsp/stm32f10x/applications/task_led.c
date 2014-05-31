#include <board.h>
#include <rtthread.h>
#include <application.h>

#include <stdlib.h>

#define SCALE_R	255
#define SCALE_G	60
#define SCALE_B	60

#define SET_R(x) TIM_SetCompare2(TIM2,(x))
#define SET_G(x) TIM_SetCompare3(TIM2,(x))
#define SET_B(x) TIM_SetCompare4(TIM2,(x))

rt_uint8_t led_stack[256];
struct rt_thread led_thread;
uint8_t Rs,Re,Gs,Ge,Bs,Be;
extern rt_event_t f_led;
	
void task_led_init(void);
void rt_thread_led_entry(void* parameter);

void set_led(uint32_t a,uint32_t b,uint32_t t)
{
	Rs=(((0x00FF0000)& a )/0x00010000)%256;
	Re=(((0x00FF0000)& b )/0x00010000)%256;
	Gs=(((0x0000FF00)& a )/0x00000100)%256;
	Ge=(((0x0000FF00)& b )/0x00000100)%256;
	Bs=(((0x000000FF)& a )/0x00000001)%256;
	Be=(((0x000000FF)& b )/0x00000001)%256;
	rt_event_send(f_led,(rt_uint32_t)(t/2));
}

void task_led_init(void)
{
	rt_err_t result;
	
	result = rt_thread_init(&led_thread,
	                        "LED",
	                        rt_thread_led_entry,
	                        RT_NULL,
	                        (rt_uint8_t*)led_stack,
	                        sizeof(led_stack),
	                        PRIO_LED,
	                        2);
	//if (result == RT_EOK) rt_thread_startup(&led_thread);
}

void rt_thread_led_entry(void* parameter)
{
	static rt_uint32_t e,t;
	static uint8_t s;
	static int32_t i;
	rt_event_recv(f_led,0xFFFFFFFF,RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR,0,&e);
	t=RT_TICK_PER_SECOND;
	while(1)
	{
		if (rt_event_recv(f_led,0xFFFFFFFF,RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR,RT_TICK_PER_SECOND/20,&e)==RT_EOK)
		{
			t=e;
			i=0;
			s=1;
		}
		SET_R(Rs+i*(Re-Rs)/t);
		SET_G(Gs+i*(Ge-Gs)/t);
		SET_B(Bs+i*(Be-Bs)/t);
		if (s==1) 
		{
			i+=20;
			if (i>t)
			{
				i-=20;
				s=0;
			}
		}else{
			i-=20;
			if (i<0)
			{
				i+=20;
				s=1;
			}
		}
	}
}
