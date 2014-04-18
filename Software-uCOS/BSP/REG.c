#include "REG.h"

static uint8_t REG_cnt=4;
static uint8_t REG_out[4]={0xBF,0xBF,0xBF,0xBF};

void REG_RCLK_Tick(void);
void REG_Push(uint8_t);

void REG_RCLK_Tick(void)
{
  GPIO_SetBits(REG_PORT,REG_RCLK);
  GPIO_ResetBits(REG_PORT,REG_RCLK);
}

void REG_Push(uint8_t ch)
{
  uint8_t i;
  for (i=8;i;i--)
  {
    GPIO_WriteBit(REG_PORT,REG_SER,ch>>7);
    ch<<=1;
    GPIO_SetBits(REG_PORT,REG_SECLK);
    GPIO_ResetBits(REG_PORT,REG_SECLK);
  }
}

void REG_Update(void)
{
  REG_cnt++;
  if (REG_cnt>=4) REG_cnt=0;
  REG_Push(1<<REG_cnt);
  REG_Push(REG_out[REG_cnt]);
  REG_RCLK_Tick();
}

void REG_Init(void)
{
  GPIO_InitTypeDef usrGPIO;
  
  RCC_APB2PeriphClockCmd(REG_RCC_CLK,ENABLE);
  usrGPIO.GPIO_Mode = GPIO_Mode_Out_PP;
  usrGPIO.GPIO_Pin = REG_SER|REG_SECLK|REG_RCLK|REG_nOE;
  usrGPIO.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(REG_PORT,&usrGPIO);
  REG_Off();
}

void REG_Set(uint8_t a,uint8_t b,uint8_t c,uint8_t d,uint8_t comment,uint8_t dot)
{
  REG_out[0]=a;
  REG_out[1]=b;
  REG_out[2]=c + (dot==0?0x80:0x00);
  REG_out[3]=d + (comment==0?0x80:0x00);
}

uint8_t REG_HexToReg(uint8_t in)
{
  uint8_t temp;
  switch(in)
  {
    case 0x00:temp=0x40;break;
    case 0x01:temp=0x79;break;
    case 0x02:temp=0x24;break;
    case 0x03:temp=0x30;break;
    case 0x04:temp=0x19;break;
    case 0x05:temp=0x12;break;
    case 0x06:temp=0x02;break;
    case 0x07:temp=0x78;break;
    case 0x08:temp=0x00;break;
    case 0x09:temp=0x10;break;
    case 0x0A:temp=0x08;break;
    case 0x0B:temp=0x03;break;
    case 0x0C:temp=0x27;break;
    case 0x0D:temp=0x21;break;
    case 0x0E:temp=0x06;break;
    case 0x0F:temp=0x0E;break;
    default: temp=0xFF;break;
  }
  return (temp|0x80);
}
