/*
 * buzzer.h
 *
 *      Author: Baram
 */

#ifndef _BUZZER_H_
#define _BUZZER_H_

#include <Arduino.h>
#include "../def.h"
#include "buzzer_pitches.h"


class Buzzer
{
  public:
    Buzzer();
    ~Buzzer();
    
    bool begin();

    void playTone(uint32_t freq, uint32_t duration_ms);
    void stopTone();
  private:
    uint32_t out_pin;    
};



#endif