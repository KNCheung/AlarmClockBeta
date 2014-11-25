#include <board.h>

void Debug_Init(void);

void Debug_Init(void)
{
	GPIO_InitTypeDef usrGPIO;
	
	usrGPIO.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	usrGPIO.GPIO_Pin = GPIO_Pin_8;
	usrGPIO.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB,&usrGPIO);
}
