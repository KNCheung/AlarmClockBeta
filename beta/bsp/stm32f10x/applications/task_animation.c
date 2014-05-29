#include <board.h>
#include <rtthread.h>
#include <application.h>

#include <stdlib.h>

uint8_t singlechar[][4]=
{
	{0x00,0x1C,0x22,0x1c},		//0
	{0x00,0x12,0x3E,0x02},		//1
	{0x00,0x26,0x2A,0x1A},		//2
	{0x00,0x22,0x2A,0x14},		//3
	{0x00,0x38,0x08,0x3E},		//4
	{0x00,0x3A,0x2A,0x24},		//5
	{0x00,0x3E,0x2A,0x2E},		//6
	{0x00,0x20,0x2E,0x30},		//7
	{0x00,0x3E,0x2A,0x3E},		//8
	{0x00,0x3A,0x2A,0x3E},		//9
	{0x00,0x00,0x02,0x00},		//Dot
	{0x00,0x08,0x08,0x08},		//Minus
	{0x00,0x00,0x00,0x00},		//Blank
	{0x00,0x00,0x14,0x00},		//Comment
};

#define BLANK_ICON {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}
uint8_t emoticon[][8] =
{
	BLANK_ICON,
	{0x10,0x3F,0x71,0xF7,0xF1,0x71,0x3F,0x10},		//House
	{0x30,0x78,0x7C,0x3E,0x3E,0x7C,0x78,0x30},		//RedHeart
	{0x30,0x48,0x44,0x22,0x22,0x44,0x48,0x30},		//Heart
	{0x00,0x04,0x22,0x02,0x02,0x22,0x04,0x00},		//Smile
	{0x00,0x10,0x30,0x7e,0x7e,0x30,0x10,0x00},		//UpArrow
	{0x00,0x02,0x24,0x04,0x04,0x24,0x02,0x00},		//Sad
	{0x00,0x00,0x00,0xF8,0x08,0x08,0x08,0x00},		//Clock
	{0x00,0x38,0x08,0x3E,0x00,0x2E,0x2A,0x3A},		//42
	{0x00,0x60,0x60,0x1C,0x22,0x22,0x14,0x00},		//CelsiusDegree	
	{0xFF,0x91,0xA7,0x85,0x85,0xA7,0x91,0xFF},		//‡å
	{0x20,0x51,0xAA,0x55,0x2A,0x15,0x2A,0x55},		//RightDownArrow
	{0x00,0x62,0x64,0x08,0x10,0x26,0x46,0x00},		//Percentage
};

#define N_APP 5
const uint8_t app_icon_index[N_APP] = 
{
	ICON_BLANK,ICON_CLOCK,ICON_REDHEART,ICON_JIONG,ICON_42
};

rt_uint8_t animation_stack[4096];
struct rt_thread animation_thread;

void task_animation_init(void);
void rt_thread_animation_entry(void* parameter);
static void ani_switch(uint8_t n,uint8_t* B,uint16_t delta);
static void ani_switch_2char(uint8_t n,uint8_t* A,uint8_t *B,uint16_t delta);

static uint8_t CurrImage[8]={0x00,0x04,0x22,0x02,0x02,0x22,0x04,0x00};

extern int16_t temperature,humidity,outdoortemperature;
extern uint8_t rtc_Y,rtc_M,rtc_D,rtc_h,rtc_m,rtc_s;

void rt_thread_animation_entry(void* parameter)
{
	extern rt_event_t f_msg;
	int8_t app,flag;
	rt_uint32_t msg;
	rt_thread_delay_hmsm(0,0,3,0);
	ani_switch(2,emoticon[ICON_BLANK],1024);
	while (1)
	{
		rt_event_recv(f_msg,0xFFFFFFFF,RT_EVENT_FLAG_OR,RT_WAITING_FOREVER,&msg);
		if (msg&F_ANI_TIME)
		{
			ani_switch_2char(2,singlechar[rtc_h/10],singlechar[rtc_h%10],768);
			ani_switch_2char(2,singlechar[CHAR_COMMENT],singlechar[rtc_m/10],768);
			ani_switch_2char(2,singlechar[rtc_m%10],singlechar[CHAR_BLANK],768);
			ani_switch(2,emoticon[ICON_BLANK],768);
		}
		if (msg&F_ANI_DATE)
		{
			ani_switch_2char(2,singlechar[rtc_M/10],singlechar[rtc_M%10],768);
			ani_switch_2char(2,singlechar[CHAR_MINUS],singlechar[rtc_D/10],768);
			ani_switch_2char(2,singlechar[rtc_D%10],singlechar[CHAR_BLANK],768);
			ani_switch(2,emoticon[ICON_BLANK],768);
		}
		if (msg&F_ANI_TEMP)
		{
			ani_switch_2char(2,singlechar[temperature/100%10],singlechar[temperature/10%10],768);
			ani_switch_2char(2,singlechar[CHAR_DOT],singlechar[temperature%10],768);
			ani_switch(2,emoticon[ICON_CELSIUS],768);
			ani_switch(2,emoticon[ICON_BLANK],768);
			ani_switch_2char(2,singlechar[humidity/100%10],singlechar[humidity/10%10],768);
			ani_switch_2char(2,singlechar[CHAR_DOT],singlechar[humidity%10],768);
			ani_switch(2,emoticon[ICON_PERCENTAGE],768);
			ani_switch(2,emoticon[ICON_BLANK],768);
		}
		if (msg&(F_ANI_PREV|F_ANI_NEXT))
		{
			app=0;
			if (msg&F_ANI_PREV)
			{
				app=(app-1+N_APP)%N_APP;
				ani_switch(6,emoticon[app_icon_index[app]],128);
			}
			else 
			{
				app=(app+1)%N_APP;
				ani_switch(7,emoticon[app_icon_index[app]],128);
			}
			rt_event_recv(f_msg,0xFFFFFFFF,RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR,0,&msg);
			flag=0;
			while (rt_event_recv(f_msg,F_ANI_NEXT|F_ANI_PREV|F_ANI_CHOOSE,RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR,RT_TICK_PER_SECOND*3,&msg)==RT_EOK)
			{
				if (msg&F_ANI_CHOOSE)
				{
					flag=1;
					break;
				}
				if (msg&F_ANI_PREV)
				{
					app=(app-1+N_APP)%N_APP;
					ani_switch(6,emoticon[app_icon_index[app]],128);
				}
				else 
				{
					app=(app+1)%N_APP;
					ani_switch(7,emoticon[app_icon_index[app]],128);
				}
			}
			if (app&&flag)
				ani_switch(1,emoticon[app_icon_index[0]],256);
			else if (app&&(!flag))
				ani_switch(0,emoticon[app_icon_index[0]],128);
		}
		rt_event_recv(f_msg,0xFFFFFFFF,RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR,0,&msg);
	}
}

void task_animation_init(void)
{
	rt_err_t result;
	result = rt_thread_init(&animation_thread,
	                        "Animation",
	                        rt_thread_animation_entry,
	                        RT_NULL,
	                        (rt_uint8_t*)animation_stack,
	                        sizeof(animation_stack),
	                        PRIO_ANIMATION,
	                        2);
	if (result == RT_EOK) rt_thread_startup(&animation_thread);
}

static void ani_switch(uint8_t n,uint8_t* B,uint16_t delta)
{
	uint8_t tB[8],i,j;
	for (i=0;i<8;i++)
		tB[i]=B[i];
	switch (n)
	{
		case 0: //MoveUpward
			for (i=0;i<8;i++)
			{
				for (j=0;j<8;j++)
				{
					CurrImage[j]=(CurrImage[j]<<1)+((tB[j]&0x80)?1:0);
					tB[j]<<=1;
				}
				PushBitMap(REG1,CurrImage); 
				rt_thread_delay_hmsm(0,0,0,(delta/8)?(delta/8):1);
			}
			break;
			
		case 1: //MoveDownward
			for (i=0;i<8;i++)
			{
				for (j=0;j<8;j++)
				{
					CurrImage[j]=(CurrImage[j]>>1)+((tB[j]%2)?0x80:0x00);
					tB[j]>>=1;
				}
				PushBitMap(REG1,CurrImage); 
				rt_thread_delay_hmsm(0,0,0,(delta/8)?(delta/8):1);
			}
			break;
			
		case 2: //MoveLeftward
			for (i=0;i<8;i++)
			{
				for (j=0;j<7;j++)
					CurrImage[j]=CurrImage[j+1];
				CurrImage[7]=tB[i];
				PushBitMap(REG1,CurrImage); 
				rt_thread_delay_hmsm(0,0,0,(delta/8)?(delta/8):1);
			}
			break;
			
		case 3: //MoveRightward
			for (i=0;i<8;i++)
			{
				for (j=7;j;j--)
					CurrImage[j]=CurrImage[j-1];
				CurrImage[0]=tB[7-i];
				PushBitMap(REG1,CurrImage); 
				rt_thread_delay_hmsm(0,0,0,(delta/8)?(delta/8):1);
			}
			break;
			
		case 4: //WipeUpward
			for (i=0;i<8;i++)
			{
				for (j=0;j<8;j++)
					CurrImage[j]=(CurrImage[j]&(~(1<<i)))|(tB[j]&(1<<i));
				PushBitMap(REG1,CurrImage); 
				rt_thread_delay_hmsm(0,0,0,(delta/8)?(delta/8):1);
			}
			break;
			
		case 5: //WipeDownward
			for (i=0;i<8;i++)
			{
				for (j=0;j<8;j++)
					CurrImage[j]=(CurrImage[j]&(~(1<<(7-i))))|(tB[j]&(1<<(7-i)));
				PushBitMap(REG1,CurrImage); 
				rt_thread_delay_hmsm(0,0,0,(delta/8)?(delta/8):1);
			}
			break;
		
		case 6: //WipeLeftward
			for (i=0;i<8;i++)
			{
				CurrImage[7-i]=tB[7-i];
				PushBitMap(REG1,CurrImage); 
				rt_thread_delay_hmsm(0,0,0,(delta/8)?(delta/8):1);
			}
			break;
		
		case 7: //WipeRightward
			for (i=0;i<8;i++)
			{
				CurrImage[i]=tB[i];
				PushBitMap(REG1,CurrImage); 
				rt_thread_delay_hmsm(0,0,0,(delta/8)?(delta/8):1);
			}
			break;
	}
}

static void ani_switch_2char(uint8_t n,uint8_t* A,uint8_t *B,uint16_t delta)
{
	uint8_t temp[8],i;
	for (i=0;i<4;i++)
	{
		temp[i]=A[i];
		temp[i+4]=B[i];
	}
	ani_switch(n,temp,delta);
}

