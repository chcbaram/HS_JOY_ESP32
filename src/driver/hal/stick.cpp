/*
 * stick.c
 *
 *  Created on: 2020. 12. 23.
 *      Author: baram
 */


#include "stick.h"




Stick::Stick()
{
  x_adc_offset = 4095/2;
  y_adc_offset = 4095/2;

  x_value = 0;
  y_value = 0;    

  dead_zone = 200;
  adc_max_value = 1700;    
}

Stick::~Stick()
{
    
}

bool Stick::begin(AdcChannel_t x_ch, AdcChannel_t y_ch)
{
  adc_x_ch = x_ch;
  adc_y_ch = y_ch;
  return true;
}

uint16_t Stick::getAdcX()
{
  return adcInfoReadRaw(adc_x_ch);
}

uint16_t Stick::getAdcY()
{
  return adcInfoReadRaw(adc_y_ch);
}

int16_t Stick::getX()
{
  update();
  return x_value;
}

int16_t Stick::getY()
{
  update();
  return y_value;
}

void Stick::update()
{
  int32_t value;
  int32_t value_out;
  uint16_t x_adc_value;
  uint16_t y_adc_value;


  x_adc_value = adcInfoReadRaw(adc_x_ch);
  y_adc_value = adcInfoReadRaw(adc_y_ch);

  value = -constrain(x_adc_value-x_adc_offset, -2000, 2000);
  if (value >  dead_zone)      value -= dead_zone;
  else if (value < -dead_zone) value += dead_zone;
  else                                value  = 0;

  value_out = constrain(value, -adc_max_value, adc_max_value);
  x_value   = map(value_out, -adc_max_value, adc_max_value, -100, 100);


  value = -constrain(y_adc_value-y_adc_offset, -2000, 2000);
  if (value >  dead_zone)      value -= dead_zone;
  else if (value < -dead_zone) value += dead_zone;
  else                                value  = 0;

  value_out = constrain(value, -adc_max_value, adc_max_value);
  y_value   = map(value_out, -adc_max_value, adc_max_value , -100, 100);   
}