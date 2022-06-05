/*
 * led.h
 *
 *      Author: Baram
 */

#ifndef _LED_H_
#define _LED_H_

#include <Arduino.h>
#include "../def.h"


class Led 
{
  public:
    Led();
    ~Led();
    
    bool begin(void);


    void on();
    void off();
    void toggle();
};


#endif