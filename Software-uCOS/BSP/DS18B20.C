#include "DS18B20.h"

uint8_t i,k;
extern uint16_t TickCnt;

#define OneWire_Pin GPIO_Pin_4
#define OneWire_Port GPIOA
#define OneWire_Port_Clock RCC_APB2Periph_GPIOA

void OneWire_Init(void)
{
  GPIO_InitTypeDef usrGPIO;
	RCC_APB2PeriphClockCmd(OneWire_Port_Clock,ENABLE);
	
	usrGPIO.GPIO_Mode = GPIO_Mode_Out_OD;
	usrGPIO.GPIO_Pin = OneWire_Pin;
	usrGPIO.GPIO_Speed = GPIO_Speed_50MHz;
}

ErrorStatus OneWire_Reset(void)
{
	GPIO_SetBits(OneWire_Port,OneWire_Pin);				//Pull Up the Wire
	Delay_us(1);																	//Wait, needless
	GPIO_ResetBits(OneWire_Port,OneWire_Pin);			//Pull Down the Wire to send out the reset signal
	Delay_us(480);																//Wait for 480us
	Delay_us(60);
	k=0;
	for (i=0;i<7;i++)
	{
		k+=GPIO_ReadInputDataBit(OneWire_Port,OneWire_Pin);
		Delay_us(10);
	}
	//while (GPIO_ReadInputDataBit(OneWire_Port,OneWire_Pin)==Bit_RESET);
	GPIO_SetBits(OneWire_Port,OneWire_Pin);
	if (k<=3)
	{
		return SUCCESS;
	}
	else
		return ERROR;
}

BitAction OneWire_ReadBit(void)
{
	BitAction x;
	
	GPIO_SetBits(OneWire_Port,OneWire_Pin);
	GPIO_ResetBits(OneWire_Port,OneWire_Pin);
	Delay_us(10);
	GPIO_SetBits(OneWire_Port,OneWire_Pin);
	Delay_us(5);
	x=(GPIO_ReadInputDataBit(OneWire_Port,OneWire_Pin))?Bit_SET:Bit_RESET;
	Delay_us(45);
	GPIO_SetBits(OneWire_Port,OneWire_Pin);
	
	return x;
}

void OneWire_WriteBit(BitAction x)
{
	GPIO_SetBits(OneWire_Port,OneWire_Pin);
	Delay_us(1);
	GPIO_ResetBits(OneWire_Port,OneWire_Pin);
	Delay_us(10);
	GPIO_WriteBit(OneWire_Port,OneWire_Pin,x);
	Delay_us(50);
	GPIO_SetBits(OneWire_Port,OneWire_Pin);
}

void OneWire_WriteChar(uint8_t cmd)
{
	for (i=0;i<8;i++)
	{
		OneWire_WriteBit((cmd % 2)?Bit_SET:Bit_RESET);
		cmd>>=1;
	}
}

uint8_t OneWire_ReadChar()
{
	uint8_t x;
	for (i=0;i<8;i++)
	{
		x>>=1;
		if (OneWire_ReadBit()==Bit_SET)
			x |= 0x80;
		else
			x &= 0x7f;
	}
	return x;
}

void DS18B20_StartConvTemp(void)
{
	OneWire_Reset();
	OneWire_WriteChar(DS18B20_SKIP_ROM);
	OneWire_WriteChar(DS18B20_CONVERT_T);
}

float DS18B20_ReadTemp(void)
{
  uint8_t M,L;
	int8_t tmp_i;
	float ans;
	OneWire_Reset();
	OneWire_WriteChar(DS18B20_SKIP_ROM);
	OneWire_WriteChar(DS18B20_READ_SCRATCHPAD);
	L=OneWire_ReadChar();
	M=OneWire_ReadChar();
	tmp_i=(M<<4)|(L>>4);
	ans=tmp_i+(L%16)/16.0f;
	return ans;
}
