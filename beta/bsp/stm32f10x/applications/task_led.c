#include <board.h>
#include <rtthread.h>
#include <application.h>

#include <stdlib.h>

rt_uint8_t led_stack[256];
struct rt_thread led_thread;
extern rt_event_t f_led;

uint8_t LED_State;

const uint8_t led_color[][3]={
	{0,0,0},		//BLACK
	{255,0,0},		//RED
	{0,80,0},		//GREEN
	{0,0,255},		//BLUE
	{255,0,80},		//PURPLE
	{0,80,100},		//CYAN
	{255,60,0},		//YELLOW
	{255,80,100},	//WHITE
};
	
void task_led_init(void);
void rt_thread_led_entry(void* parameter);
void change_led(uint8_t j,uint8_t k);

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
	if (result == RT_EOK) rt_thread_startup(&led_thread);
}

void rt_thread_led_entry(void* parameter)
{
	static rt_uint32_t e;
	static uint8_t s;
	static int32_t i,j,k;
	extern rt_mutex_t m_display;
	rt_event_recv(f_led,0xFFFFFFFF,RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR,0,&e);
	LED_RGB(0,0,0);
			
	while(1)
	{
		if (rt_event_recv(f_led,0xFFFFFFFF,RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR,RT_TICK_PER_SECOND*5,&e)==RT_EOK)
		{
			j=k;
			k=0;
			while (e)
			{
				e>>=1;
				k++;
			}
			k--;
			change_led(j,k);
		}else{
			if (rt_mutex_take(m_display,0)==RT_EOK)
			{
				rt_mutex_release(m_display);
				j=k;
				do
				{
					k=rand()%8;
				}while((j==k)||(k==0));
				change_led(j,k);
			}
		}
	}
}

void change_led(uint8_t j,uint8_t k)
{
	uint8_t i;
	if (j==k) return;
	LED_State = LED_CHANGING;
	for (i=10;i<=20;i++)
	{
		LED_RGB((led_color[k][0]-led_color[j][0])/20*i+led_color[j][0],(led_color[k][1]-led_color[j][1])/20*i+led_color[j][1],(led_color[k][2]-led_color[j][2])/20*i+led_color[j][2]);
		rt_thread_delay(2);
	}
	LED_RGB(led_color[k][0],led_color[k][1],led_color[k][2]);
	
	LED_State = LED_CHANGED;
}
