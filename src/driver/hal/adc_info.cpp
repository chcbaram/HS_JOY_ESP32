/*
 * adc_info.h
 *
 *      Author: Baram
 */

#include "adc_info.h"





typedef struct
{
  int pin_number;
  bool enable;
  uint16_t value;
  uint16_t value_raw;
} adc_pin_info_t;


adc_pin_info_t adc_pin_info[ADC_CH_MAX] =
{  
  {36, true, 0, 0},
  {39, true, 0, 0},
  {34, true, 0, 0},
  {35, true, 0, 0},
};

void adcInfoInit(void)
{
}

void adcInfoEnable(AdcChannel_t ch)
{
  adc_pin_info[ch].enable = true;
}

void adcInfoDisable(AdcChannel_t ch)
{
  adc_pin_info[ch].enable = false;
}

void adcInfoUpdate(void)
{
  uint16_t value;


  for (int i=0; i<ADC_CH_MAX; i++)
  {
    if (adc_pin_info[i].enable == true)
    {
      adc_pin_info[i].value_raw = analogRead(adc_pin_info[i].pin_number);
      value = constrain(adc_pin_info[i].value_raw, 0, 3000);
      value = map(value, 0, 3000, 0, 255);
      adc_pin_info[i].value = value;      
    }
  }  
}

uint16_t adcInfoRead(AdcChannel_t ch)
{
  uint16_t ret = 0;

  ret = adc_pin_info[ch].value;

  return ret;
}

uint16_t adcInfoReadRaw(AdcChannel_t ch)
{
  uint16_t ret = 0;

  ret = adc_pin_info[ch].value_raw;

  return ret;
}

uint16_t adcInfoReadNow(AdcChannel_t ch)
{
  uint16_t ret = 0;

  adc_pin_info[ch].value_raw = analogRead(adc_pin_info[ch].pin_number);
  ret = adc_pin_info[ch].value_raw;
  return ret;
}
