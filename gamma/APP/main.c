#include <main.h>

uint8_t SYS_BUSY=0;
uint32_t SYS_TICK=0;

static struct pt pt_clock;

void fnInitializeBoard(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB,ENABLE);
	cfgTIM2();
	cfgTIM4();
	IIC_Init();
	IR_Init();
	REG2_Init(2);
}

void fnInitializeTasks(void)
{
	PT_INIT(&pt_clock);
}

void fnScheduler(void)
{
	SYS_BUSY=1;
	task_clock(&pt_clock);
	SYS_BUSY=0;
}

int main(void)
{
	SysTick_Config(SystemCoreClock / 100);
	fnInitializeBoard();
	fnInitializeTasks();
	while(1)
	{
	}
}
