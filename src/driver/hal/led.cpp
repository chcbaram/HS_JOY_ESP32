/*
 * led.c
 *
 *  Created on: 2020. 12. 23.
 *      Author: baram
 */


#include "led.h"




Led::Led()
{
    
}

Led::~Led()
{
    
}

bool Led::begin(void)
{
  pinMode(JOY_LED_PIN, OUTPUT);
  return true;
}

void Led::on()
{
  digitalWrite(JOY_LED_PIN, LOW);
}

void Led::off()
{
  digitalWrite(JOY_LED_PIN, HIGH);
}

void Led::toggle()
{
  digitalWrite(JOY_LED_PIN, !digitalRead(JOY_LED_PIN));
}
