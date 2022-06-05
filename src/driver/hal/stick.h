/*
 * stick.h
 *
 *      Author: Baram
 */

#ifndef _STICK_H_
#define _STICK_H_

#include <Arduino.h>
#include "../def.h"
#include "adc_info.h"



class Stick 
{
  public:
    Stick();
    ~Stick();
    
    bool begin(AdcChannel_t x_ch, AdcChannel_t y_ch);

    int16_t getX();
    int16_t getY();
    uint16_t getAdcX();
    uint16_t getAdcY();

  private:
    void update();

    AdcChannel_t adc_x_ch;
    AdcChannel_t adc_y_ch;

    int32_t x_adc_offset;
    int32_t y_adc_offset;

    int32_t x_value;
    int32_t y_value;    


    int32_t dead_zone;
    int32_t adc_max_value;    
};


#endif