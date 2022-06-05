/*
 * stick.h
 *
 *      Author: Baram
 */

#ifndef _STICK_H_
#define _STICK_H_

#include <Arduino.h>
#include "../def.h"


class Stick 
{
  public:
    Stick();
    ~Stick();
    
    bool begin(void);
};


#endif