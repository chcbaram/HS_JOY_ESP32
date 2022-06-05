/*
 * battery.c
 *
 *  Created on: 2020. 12. 23.
 *      Author: baram
 */


#include "battery.h"
#include "adc_info.h"


#define BAT_ADC_MAX_COUNT     100


static int32_t bat_level = 0;
static int32_t bat_vol = 0;
static const uint32_t bat_max = 405;
static const uint32_t bat_min = 310;

static uint32_t pre_time = 0;
static uint32_t buf_index = 0;
static uint32_t last_level = 0;

static uint32_t adc_data[BAT_ADC_MAX_COUNT];



Battery::Battery()
{
    
}

Battery::~Battery()
{
    
}

bool Battery::begin(void)
{
  uint32_t i;

  for (i=0; i<BAT_ADC_MAX_COUNT; i++)
  {
    adc_data[buf_index] = adcInfoReadNow(ADC_CH_VBAT);
    buf_index = (buf_index + 1) % BAT_ADC_MAX_COUNT;
  }  
  return true;
}

void Battery::update(void)
{
  uint32_t i;
  int32_t sum;
  int32_t value = 0;
  int32_t vol = 0;
  int32_t avg;


  if (millis()-pre_time >= 10)
  {
    pre_time = millis();

    adc_data[buf_index] = adcInfoReadRaw(ADC_CH_VBAT);
    buf_index = (buf_index + 1) % BAT_ADC_MAX_COUNT;


    sum = 0;
    for (i=0; i<BAT_ADC_MAX_COUNT; i++)
    {
      sum += adc_data[i];
    }

    avg = sum/BAT_ADC_MAX_COUNT;
    bat_vol = 660 * avg / 4095;
    vol   = constrain(bat_vol, bat_min, bat_max);

    value = map(vol, bat_min, bat_max, 0, 100);
    if (abs((double)(value-last_level)) > 5 || value == 100)
    {
      bat_level = value;
      last_level = value;
    }
  }
}

float Battery::getVoltage(void)
{
  float value;

  value = adcInfoReadRaw(ADC_CH_VBAT);

  value = (3.3f * 2.0f) * value / 4095.f;  

  return value;
}

int32_t Battery::getLevel(void)
{
  return bat_level;
}
