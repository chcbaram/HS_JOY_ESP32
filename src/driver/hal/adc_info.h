/*
 * adc_info.h
 *
 *      Author: Baram
 */

#ifndef _ADCINFO_H_
#define _ADCINFO_H_

#include <Arduino.h>


typedef enum
{
  ADC_CH_VBAT,
  ADC_CH_STICK_L_X,
  ADC_CH_STICK_L_Y,
  ADC_CH_STICK_R_X,
  ADC_CH_STICK_R_Y,
  ADC_CH_MAX
} AdcChannel_t;




void adcInfoInit(void);
void adcInfoEnable(uint16_t ch);
void adcInfoDisable(uint16_t ch);
void adcInfoUpdate(void);
uint16_t adcInfoRead(uint16_t ch);
uint16_t adcInfoReadRaw(uint16_t ch);

#endif