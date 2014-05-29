#include <board.h>

#define AM2302_PORT			GPIOB
#define AM2302_PIN			GPIO_Pin_14

#define AM2302_HIGH			GPIO_SetBits(AM2302_PORT, AM2302_PIN)
#define AM2302_LOW			GPIO_ResetBits(AM2302_PORT, AM2302_PIN)
#define AM2302_READ			GPIO_ReadInputDataBit(AM2302_PORT, AM2302_PIN)
#define AM2302_GETTIME		TIM_GetCounter(TIM4)

void AM2302_Init(void);
ErrorStatus AM2302_Read(int16_t* humi,int16_t* temp);
extern void Delay_us(uint16_t);

uint8_t AM2302_ReadByte(void);
ErrorStatus AM2302_Start(void);

static ErrorStatus AM2302_ERROR;
	
ErrorStatus AM2302_Read(int16_t* humi,int16_t* temp)
{
	uint8_t t[5],i;
	if (AM2302_Start()==ERROR) return ERROR;
	AM2302_ERROR = SUCCESS;
	
	for (i=0;i<5;i++)
	{
		t[i]=AM2302_ReadByte();
		if (AM2302_ERROR==ERROR)
			return ERROR;
	}
	if ((uint8_t)(t[0]+t[1]+t[2]+t[3])!=t[4]) return ERROR;
	*humi = t[0]*0x0100+t[1];
	*temp = t[2]*0x0100+t[3];
	return SUCCESS;
}

uint8_t AM2302_ReadByte(void)
{
	uint8_t i,ch;
	uint16_t ts,to;
	ch = 0;
	to = AM2302_GETTIME;
	while ((AM2302_READ==1)&&((uint16_t)(AM2302_GETTIME-to)<500));
	if (AM2302_READ==1)
	{
		AM2302_ERROR=ERROR;
		return 0xFF;
	}
	ts = AM2302_GETTIME;
	for (i=8;i;i--)
	{
		ch<<=1;
		ts = AM2302_GETTIME;
		
		to = AM2302_GETTIME;
		while ((AM2302_READ==0)&&((uint16_t)(AM2302_GETTIME-to)<500));
		if (AM2302_READ==0)
		{
			AM2302_ERROR=ERROR;
			return 0xFF;
		}
		
		to = AM2302_GETTIME;
		while ((AM2302_READ==1)&&((uint16_t)(AM2302_GETTIME-to)<500));
		if (AM2302_READ==1)
		{
			AM2302_ERROR=ERROR;
			return 0xFF;
		}
		
		if ((uint16_t)(AM2302_GETTIME-ts)>=100)
			ch++;
		ts = AM2302_GETTIME;
	}
	return ch;
}

ErrorStatus AM2302_Start(void)
{
	uint16_t ts;
	AM2302_LOW;
	Delay_us(1000);
	AM2302_HIGH;
	
	ts = AM2302_GETTIME;
	while ((AM2302_READ == 1)&&((uint16_t)(AM2302_GETTIME-ts)<500));
	if (AM2302_READ == 1) return ERROR;
	while (AM2302_READ == 0);
	return SUCCESS;
}

void AM2302_Init(void)
{
	GPIO_InitTypeDef usrGPIO;
	cfgTIM();
	usrGPIO.GPIO_Mode = GPIO_Mode_Out_OD;
	usrGPIO.GPIO_Pin = AM2302_PIN;
	usrGPIO.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(AM2302_PORT,&usrGPIO);
		
	AM2302_Start();
}
