/*
 * button.h
 *
 *      Author: Baram
 */

#ifndef _BUTTON_H_
#define _BUTTON_H_

#include <Arduino.h>
#include "../def.h"

typedef enum
{
  BUTTON_A,
  BUTTON_B,
  BUTTON_X,
  BUTTON_Y,
  BUTTON_LEFT,
  BUTTON_RIGHT,
  BUTTON_UP,
  BUTTON_DOWN,
  BUTTON_START,
  BUTTON_MAX
} ButtonName_t;

class Button 
{
  public:
    Button() { is_init = false; }
    ~Button();
    
    bool begin(void);


    bool isClicked(ButtonName_t btn_name);
    bool isPressed(ButtonName_t btn_name);
    bool isPressed(int btn_name) { return isPressed((ButtonName_t)btn_name); };
    void clearEvent(void);
    void clearEvent(ButtonName_t btn_name);

    uint32_t getData();
    bool update();
    bool isInit() { return is_init; };
  private:
    bool is_init;
};


#endif