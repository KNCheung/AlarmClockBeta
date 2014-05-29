#include <board.h>

#define REG2_PORT		GPIOB
#define REG2_DIN		GPIO_Pin_10
#define REG2_nCS		GPIO_Pin_11
#define REG2_CLK		GPIO_Pin_12

static uint8_t REG_NUM;

void ShiftOut(GPIO_TypeDef *GPIOx, uint16_t SDA,uint16_t SCL, uint8_t order, uint8_t data)
{
	uint8_t i;
	for (i=8;i>0;i--)
		if (order==MSBFIRST)
		{
			if (data&0x80)
				GPIO_SetBits(GPIOx,SDA);
			else
				GPIO_ResetBits(GPIOx,SDA);
			SHIFT_DELAY;
			GPIO_SetBits(GPIOx,SCL);
			SHIFT_DELAY;
			GPIO_ResetBits(GPIOx,SCL);
			SHIFT_DELAY;
			data<<=1;
		}else{
			if (data&0x01)
				GPIO_SetBits(GPIOx,SDA);
			else
				GPIO_ResetBits(GPIOx,SDA);
			SHIFT_DELAY;
			GPIO_SetBits(GPIOx,SCL);
			SHIFT_DELAY;
			GPIO_ResetBits(GPIOx,SCL);
			SHIFT_DELAY;
			data>>=1;
		}
	return;
}

void REG2_Init(uint8_t n)
{
	GPIO_InitTypeDef usrGPIO;
	usrGPIO.GPIO_Mode = GPIO_Mode_Out_OD;
	usrGPIO.GPIO_Pin = REG2_DIN | REG2_CLK | REG2_nCS;
	usrGPIO.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(REG2_PORT,&usrGPIO);
	GPIO_SetBits(REG2_PORT,REG2_nCS);
	REG_NUM = n;
	
	PushREG(REGA,REG_Shut,0x00,0x00);
	PushREG(REGA,REG_Test,0x00,0x00);
	PushREG(REGA,REG_Dec,0x00,0x0f);
	PushREG(REGA,REG_Sca,0x07,0x03);
	PushREG(0,REG_NONE);
	PushREG(REGA,REG_Int,0x03,0x01);
	PushREG(REGA,REG_X1,0x00,0x0C);
	PushREG(REGA,REG_X2,0x04,0x0B);
	PushREG(REGA,REG_X3,0x22,0x0D);
	PushREG(REGA,REG_X4,0x02,0x0E);
	PushREG(REG1,REG_X5,0x02);
	PushREG(REG1,REG_X6,0x22);
	PushREG(REG1,REG_X7,0x04);
	PushREG(REG1,REG_X8,0x00);
	PushREG(REGA,REG_Shut,0x01,0x01);
}

void PushREG(uint8_t n,uint8_t REG, ...)
{
	va_list arg;
	uint8_t i;
	
	va_start(arg,REG);
	
	GPIO_ResetBits(REG2_PORT,REG2_nCS);
	for (i=0;i<REG_NUM;i++)
	{
		if (n & (1<<i))
		{
			ShiftOut(REG2_PORT,REG2_DIN,REG2_CLK,MSBFIRST,REG);
			ShiftOut(REG2_PORT,REG2_DIN,REG2_CLK,MSBFIRST,va_arg(arg,uint8_t));
		}else{
			ShiftOut(REG2_PORT,REG2_DIN,REG2_CLK,MSBFIRST,REG_NONE);
			ShiftOut(REG2_PORT,REG2_DIN,REG2_CLK,MSBFIRST,0);
		}
	}
	GPIO_SetBits(REG2_PORT,REG2_nCS);
	
	va_end(arg);
}

void PushBitMap(uint8_t n,uint8_t *BitMap)
{
	uint8_t i;
	for (i=0;i<8;i++)
		PushREG(n,i+1,BitMap[i]);
}
