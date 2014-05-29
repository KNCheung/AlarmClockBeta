#include <task.h>

uint8_t Y,M,D,h,m,s;

int task_clock(struct pt *pt)
{
	PT_BEGIN(pt);
	while (1)
	{
		Y=IIC_Read(DS3231_ADDRESS,DS3231_YEAR);		Y=(Y>>4)%10*10+Y%10;
		M=IIC_Read(DS3231_ADDRESS,DS3231_MONTH);	M=(M>>4)%2*10+M%10;
		D=IIC_Read(DS3231_ADDRESS,DS3231_DAY);		D=(D>>4)%4*10+D%10;
		h=IIC_Read(DS3231_ADDRESS,DS3231_HOUR);		h=(h>>4)%4*10+h%10;
		m=IIC_Read(DS3231_ADDRESS,DS3231_MINUTE);	m=(m>>4)%6*10+m%10;
		s=IIC_Read(DS3231_ADDRESS,DS3231_SECOND);	s=(s>>4)%6*10+s%10;
		PushREG(REG2,REG_X1,h/10);
		PushREG(REG2,REG_X2,(h%10));
		PushREG(REG2,REG_X3,m/10);
		PushREG(REG2,REG_X4,m%10);
		PT_TIMER_DELAY(pt,(60-s)*1000);
	}
	PT_END(pt);
}
