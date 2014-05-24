#include <stm32f10x.h>
#include <rtthread.h>

char ADC_Running;
void ADC_Enable(void)
{
  ADC_InitTypeDef usrADC;
  GPIO_InitTypeDef usrGPIO;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
  
  usrGPIO.GPIO_Mode = GPIO_Mode_AIN;
  usrGPIO.GPIO_Pin = GPIO_Pin_7;
  usrGPIO.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOA,&usrGPIO);
  
  usrADC.ADC_ContinuousConvMode = ENABLE;
  usrADC.ADC_DataAlign = ADC_DataAlign_Right;
  usrADC.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  usrADC.ADC_Mode = ADC_Mode_Independent;
  usrADC.ADC_NbrOfChannel = 1;
  usrADC.ADC_ScanConvMode = DISABLE;
  ADC_Init(ADC1,&usrADC);
  
  ADC_RegularChannelConfig(ADC1,ADC_Channel_7,1,ADC_SampleTime_239Cycles5);
  ADC_Cmd(ADC1,ENABLE);
}

void ADC_Disable(void)
{
  GPIO_InitTypeDef usrGPIO;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,DISABLE);
  usrGPIO.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  usrGPIO.GPIO_Pin = GPIO_Pin_0;
  usrGPIO.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB,&usrGPIO);
}

uint16_t GetVoltage(void)
{
  uint16_t tmp[8],t;
  uint8_t i,j;
  ADC_Running = 1;
  ADC_Enable();
  ADC_ResetCalibration(ADC1);
  rt_thread_delay_hmsm(0,0,0,5);
  ADC_StartCalibration(ADC1);
  rt_thread_delay_hmsm(0,0,0,5);
  
  ADC_SoftwareStartConvCmd(ADC1,ENABLE);
  for (i=0;i<8;i++)
  {
    rt_thread_delay_hmsm(0,0,0,5);
    tmp[i]=ADC_GetConversionValue(ADC1);
  }
  for (i=0;i<7;i++)
    for (j=i+1;j<8;j++)
      if (tmp[i]>tmp[j])
      {
        t=tmp[i];tmp[i]=tmp[j];tmp[j]=t;
      }
  ADC_SoftwareStartConvCmd(ADC1,DISABLE);
  
  ADC_Disable();
  ADC_Running = 0;
  t = (tmp[2]+tmp[3]+tmp[4]+tmp[5])/4;
  t = (uint16_t)(t*0.0325447f);
  return t;
}
