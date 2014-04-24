#ifndef __DS18B20_H_
#define __DS18B20_H_

#include <stdint.h>
//====REGISTER====
//====COMMAND====
#define DS18B20_SEARCH_ROM		0XF0
#define DS18B20_READ_ROM			0X33
#define DS18B20_MATCH_ROM			0X55
#define DS18B20_SKIP_ROM			0XCC
#define DS18B20_ALARM_SEARCH	0XEC
#define DS18B20_CONVERT_T			0X44
#define DS18B20_WRITE_SCRATCHPAD		0X4E
#define DS18B20_READ_SCRATCHPAD			0XBE
#define DS18B20_COPY_SCRATCHPAD			0X48
#define DS18B20_RECALL				0XB8
#define DS18B20_READ_POWER_SUPPLY		0XB4

void OneWire_Init(void);
void DS18B20_StartConvTemp(void);
float DS18B20_ReadTemp(void);

#endif
