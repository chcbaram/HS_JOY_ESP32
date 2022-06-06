/*
 * HS_JOY_ESP32.h
 *
 *  Created on: May 16, 2018
 *      Author: Baram
 */

#ifndef HS_JOY_ESP32_H_
#define HS_JOY_ESP32_H_

#include <Arduino.h>

#include "driver/driver.h"









class HS_JOY_ESP32
{
  public:
    HS_JOY_ESP32();
    ~HS_JOY_ESP32();
        

    Led       led;
    OLed      lcd;
    Button    button;
    Battery   battery;
    Menu      menu;
    Stick     stickL;
    Stick     stickR;
    Buzzer    buzzer;

    bool begin(int baud);        

    bool update(void);

  private:    
    uint32_t pre_time[8];
    bool printInitLog(const char *str_msg, bool ret);
    bool is_init;
};

extern HS_JOY_ESP32 hs_joy;

#endif 
