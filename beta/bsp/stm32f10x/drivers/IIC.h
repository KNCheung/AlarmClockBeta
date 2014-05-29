#ifndef __IIC_H_
#define __IIC_H_

#include "board.h"

void IIC_Init(void);
uint8_t IIC_Read(uint8_t Address,uint8_t Register);
void IIC_Write(uint8_t Address,uint8_t Register,uint8_t Data);
// DS3231
  #define DS3231_ADDRESS    0xD0
  #define DS3231_SECOND     0x00
  #define DS3231_MINUTE     0x01
  #define DS3231_HOUR       0x02
  #define DS3231_WEEKDAY    0x03
  #define DS3231_DAY        0x04
  #define DS3231_MONTH      0x05
  #define DS3231_YEAR       0x06
  #define DS3231_A1_S       0x07
  #define DS3231_A1_M       0x08
  #define DS3231_A1_H       0x09
  #define DS3231_A1_D       0x0A
  #define DS3231_A2_M       0x0B
  #define DS3231_A2_H       0x0C
  #define DS3231_A2_D       0x0D
  #define DS3231_CONTROL    0x0E
  #define DS3231_CON_STA    0x0F
  #define DS3231_OFFSET     0x10
  #define DS3231_TEMP_MSB   0x11
  #define DS3231_TEMP_LSB   0x12
  
//AT24C32
  #define AT24C32_ADDRESS   0xAE
  
#endif
