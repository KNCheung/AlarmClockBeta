#include "REG.h"

static rt_uint8_t REG_cnt=4;
rt_uint8_t REG_out[4]={0xBF,0xBF,0xBF,0xBF};

void REG_RCLK_Tick(void);
void REG_Push(rt_uint8_t);

void REG_RCLK_Tick(void)
{
  GPIO_SetBits(REG_PORT,REG_RCLK);
  GPIO_ResetBits(REG_PORT,REG_RCLK);
}

void REG_Push(rt_uint8_t ch)
{
  rt_uint8_t i;
  for (i=8;i;i--)
  {
    GPIO_WriteBit(REG_PORT,REG_SER,(ch>>7)?Bit_SET:Bit_RESET);
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
	usrGPIO.GPIO_Pin = REG_SER|REG_SECLK|REG_RCLK;
	usrGPIO.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(REG_PORT,&usrGPIO);
	
	usrGPIO.GPIO_Mode = GPIO_Mode_Out_OD;
	usrGPIO.GPIO_Pin = REG_nOE;
	usrGPIO.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(REG_PORT,&usrGPIO);

	REG_Off();
}

uint32_t REG_Convert(rt_uint8_t a,rt_uint8_t b,rt_uint8_t c,rt_uint8_t d,rt_uint8_t comment,rt_uint8_t dot)
{
  return (a*0x00000001+b*0x00000100+(c + (dot?0x80:0x00))*0x00010000+(d + (comment?0x80:0x00))*0x01000000);
}

void REG_Set(uint32_t setting)
{
	REG_out[0] = setting & 0xFF;
	REG_out[1] = (setting>>8) & 0xFF;
	REG_out[2] = (setting>>16) & 0xFF;
	REG_out[3] = (setting>>24) & 0xFF;
}

rt_uint8_t REG_HexToReg(rt_uint8_t in)
{
  rt_uint8_t temp;
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
  return (temp&0x7F);
}
