#include "IIC.h"

#define IIC_PORT GPIOA
#define IIC_GPIO_CLK RCC_APB2Periph_GPIOA
#define IIC_SDA  GPIO_Pin_11
#define IIC_SCL  GPIO_Pin_12

#define SDA_H   GPIO_SetBits(IIC_PORT,IIC_SDA)
#define SDA_L   GPIO_ResetBits(IIC_PORT,IIC_SDA)
#define SCL_H   GPIO_SetBits(IIC_PORT,IIC_SCL)
#define SCL_L   GPIO_ResetBits(IIC_PORT,IIC_SCL)
#define SDA_Read  GPIO_ReadInputDataBit(IIC_PORT,IIC_SDA)

#define IIC_Transmitter   0x00
#define IIC_Receiver      0x01

static void IIC_Delay(void);
static void IIC_Start(void);
static void IIC_Stop(void);
static void IIC_WriteByte(uint8_t Data);
static uint8_t IIC_ReadByte(void);
static uint8_t IIC_GetACK(void);

static void IIC_Start(void) 
{ 
  SDA_H; 
  IIC_Delay(); 
  SCL_H; 
  IIC_Delay(); 
  SDA_L; 
  IIC_Delay(); 
  SDA_L; 
  IIC_Delay(); 
} 

void IIC_Stop(void) 
{ 
  SCL_L; 
  IIC_Delay(); 
  SDA_L; 
  IIC_Delay(); 
  SCL_H; 
  IIC_Delay(); 
  SDA_H; 
  IIC_Delay(); 
} 

static void IIC_WriteByte(uint8_t Data)
{
  uint8_t i;
  for (i=0;i<8;i++)
  {
    SCL_L;
    IIC_Delay();
    if (Data&0x80)
      SDA_H;
    else 
      SDA_L;
    Data<<=1;
    IIC_Delay(); 
    SCL_H;
    IIC_Delay(); 
  }
  SCL_L;
}

static uint8_t IIC_ReadByte(void)
{
  u8 i=8; 
  u8 ReceiveByte=0; 
  
  SDA_H; 
  while(i--) 
  { 
    ReceiveByte<<=1;       
    SCL_L; 
    IIC_Delay(); 
    SCL_H; 
    IIC_Delay(); 
    if(SDA_Read) 
      ReceiveByte|=0x01; 
  } 
  SCL_L; 
  return ReceiveByte;
}

static uint8_t IIC_GetACK(void)
{
  SCL_L; 
  IIC_Delay(); 
  SDA_H; 
  IIC_Delay(); 
  SCL_H; 
  IIC_Delay(); 
  if(SDA_Read) 
  { 
    SCL_L; 
    return ERROR; 
  } 
  SCL_L; 
  return SUCCESS; 
} 

static void IIC_Delay(void)
{
  uint8_t i=5;
  while (i--);
}

void IIC_Init(void)
{
  GPIO_InitTypeDef usrGPIO;
  
  RCC_APB2PeriphClockCmd(IIC_GPIO_CLK,ENABLE);
  
  usrGPIO.GPIO_Mode = GPIO_Mode_Out_OD;
  usrGPIO.GPIO_Pin = IIC_SCL | IIC_SDA;
  usrGPIO.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(IIC_PORT,&usrGPIO);
}

uint8_t IIC_Read(uint8_t Address,uint8_t Register)
{  
  uint8_t ch;
  IIC_Start();
  IIC_WriteByte(Address+IIC_Transmitter);
  IIC_GetACK();
  IIC_WriteByte(Register);
  IIC_GetACK();
  
  IIC_Start();
  IIC_WriteByte(Address+IIC_Receiver);
  IIC_GetACK();
  ch=IIC_ReadByte();
  IIC_Stop();
  return ch;
}

void IIC_Write(uint8_t Address,uint8_t Register,uint8_t Data)
{  
  IIC_Start();
  IIC_WriteByte(Address+IIC_Transmitter);
  IIC_GetACK();
  IIC_WriteByte(Register);
  IIC_GetACK();
  IIC_WriteByte(Data);
  IIC_GetACK();
  IIC_Stop();
}
