/*
 * battery.c
 *
 *  Created on: 2020. 12. 23.
 *      Author: baram
 */


#include "battery.h"
#include "adc_info.h"





Battery::Battery()
{
    
}

Battery::~Battery()
{
    
}

bool Battery::begin(void)
{
  return true;
}

float Battery::getVoltage(void)
{
  float value;

  value = adcInfoReadRaw(ADC_CH_VBAT);

  value = (3.3f * 2.0f) * value / 4095.f;  

  return value;
}