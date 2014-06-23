#include <board.h>
#include <rtthread.h>

#ifdef  RT_USING_COMPONENTS_INIT
#include <components.h>
#endif  /* RT_USING_COMPONENTS_INIT */

#include <application.h>

void reboot(void)
{
	rt_kprintf("=====REBOOTED=====");
	rt_thread_delay_hmsm(0,0,0,10);  
	NVIC_SystemReset();
	return;
}
FINSH_FUNCTION_EXPORT(reboot ,Reboot the MCU)

void fnDebug(uint16_t x)
{	
}
FINSH_FUNCTION_EXPORT_ALIAS(fnDebug,debug,Debug Function)

/*
void Print_Setting(void)
{
	uint8_t i;
	for (i=0;i<SETTING_LENGTH;i++)
	{
		if (i%8==0)
			rt_kprintf("\n");
		rt_kprintf("\t%d ",Setting[i]);
	}
	rt_kprintf("\n");
	return;
}
FINSH_FUNCTION_EXPORT_ALIAS(Print_Setting,setting,Print Setting Values)*/

/*
void Setting_Write(void)
{
	uint8_t t;
	while (1)
	{
		IIC_WriteSeq(AT24C32_ADDRESS,SETTING_BASE,Setting,SETTING_LENGTH);
		rt_thread_delay_hmsm(0,0,0,100);
		IIC_ReadSeq(AT24C32_ADDRESS,SETTING_BASE+1, &t,1);
		if (t!=Setting[1])
			rt_thread_delay_hmsm(0,0,0,100);
		else
			break;
	}
	return;
}

void Setting_Read(void)
{
	IIC_ReadSeq(AT24C32_ADDRESS,SETTING_BASE,Setting,SETTING_LENGTH);
	return;
}
void pomodoro_set(uint8_t a,uint8_t b,uint8_t c)
{
	Setting[POMODORO_TIME] = a;
	Setting[POMODORO_BREAK_TIME] = b;
	Setting[POMODORO_REST_TIME] = c;
	Setting_Write();
	return;
}
FINSH_FUNCTION_EXPORT_ALIAS(pomodoro_set,pomodoro,Set Interval of Pomodoro Clock(Working,Break,Rest))
*/
void time(void)
{
	rt_enter_critical();
	rt_kprintf("         Date:\t20%x-%x-%x\n         Time:\t%x:%x:%x\n  Temperature:\t%d C\n",\
	                      IIC_Read(DS3231_ADDRESS, DS3231_YEAR),IIC_Read(DS3231_ADDRESS,DS3231_MONTH)&0x7f,IIC_Read(DS3231_ADDRESS,DS3231_DAY),\
	                      IIC_Read(DS3231_ADDRESS, DS3231_HOUR),IIC_Read(DS3231_ADDRESS,DS3231_MINUTE),IIC_Read(DS3231_ADDRESS,DS3231_SECOND),\
	                      IIC_Read(DS3231_ADDRESS, DS3231_TEMP_MSB));
	rt_exit_critical();
	return;
}
FINSH_FUNCTION_EXPORT(time,Read DS3231)

void StandBy(void)
{
	PushREG(REGA,REG_Shut,0x00,0x00);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE); 
	NVIC_SystemLPConfig(NVIC_LP_SEVONPEND,ENABLE);
	PWR_WakeUpPinCmd(ENABLE);
	PWR_ClearFlag(PWR_FLAG_WU);
	PWR_ClearFlag(PWR_FLAG_SB);
	PWR_EnterSTANDBYMode();
	return;
}
FINSH_FUNCTION_EXPORT(StandBy , Low Power Mode)

void Set_Time(uint8_t Y,uint8_t M,uint8_t D,uint8_t w, uint8_t h,uint8_t m, uint8_t s)
{
  IIC_Write(DS3231_ADDRESS, DS3231_SECOND , ((s/10)<<4)|(s%10));
  IIC_Write(DS3231_ADDRESS, DS3231_MINUTE , ((m/10)<<4)|(m%10));
  IIC_Write(DS3231_ADDRESS, DS3231_HOUR , ((h/10)<<4)|(h%10));
  IIC_Write(DS3231_ADDRESS, DS3231_WEEKDAY , ((w/10)<<4)|(w%10));
  IIC_Write(DS3231_ADDRESS, DS3231_YEAR , ((Y/10)<<4)|(Y%10));
  IIC_Write(DS3231_ADDRESS, DS3231_MONTH , ((M/10)<<4)|(M%10));
  IIC_Write(DS3231_ADDRESS, DS3231_DAY , ((D/10)<<4)|(D%10));  
  time();
  return;
}
FINSH_FUNCTION_EXPORT(Set_Time , Set time(YY,MM,DD,w,hh,mm,ss))

//FINSH_FUNCTION_EXPORT_ALIAS(fnSetAlarmClock,alarm,Set Alarm Clock(hh,mm,ss))
/*
void ResetEEPROM(void)
{
	Setting[0] = 42;
	Setting_Write();
	return;
}
FINSH_FUNCTION_EXPORT(ResetEEPROM , Reset EEPROM )
*/

void SetLed(uint8_t x)
{
	extern rt_event_t f_led;
	rt_event_send(f_led,(1<<(x-1)));
}
FINSH_FUNCTION_EXPORT(SetLed , Set LED value )
