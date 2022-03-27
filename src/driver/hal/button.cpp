/*
 * button.c
 *
 *  Created on: 2020. 12. 23.
 *      Author: baram
 */


#include "button.h"


#define BUTTON_MAX_CH         9


typedef struct
{
  uint8_t  ch;
  uint8_t  state;
  uint8_t  state_pre;
  uint8_t  state_next;
  uint32_t pressed_time;
  uint32_t repeat_start_time;
  uint32_t repeat_pressed_time;
  uint32_t pre_time;

  uint8_t event_flag;
  uint8_t state_flag;
  uint8_t click_count;
} button_obj_t;

enum ButtonEvt
{
  BUTTON_EVT_PRESSED  = (1<<0),
  BUTTON_EVT_RELEASED = (1<<1),
  BUTTON_EVT_CLICKED  = (1<<2),
  BUTTON_EVT_REPEATED = (1<<3),
};

enum ButtonState
{
  BUTTON_STATE_PRESSED  = (1<<0),
  BUTTON_STATE_RELEASED = (1<<1),
  BUTTON_STATE_REPEATED = (1<<2),
};



typedef struct
{
  uint8_t       pull_up;
  uint32_t      pin;  
  uint8_t       on_state;
} button_tbl_t;


button_tbl_t button_tbl[BUTTON_MAX_CH] =
    {
        {1, 13, 0}, // LEFT
        {1, 14, 0}, // RIGHT
        {1, 15, 0}, // UP
        {1, 16, 0}, // DOWN
        {0, 17, 1}, // START
        {1, 23, 0}, // A
        {1, 25, 0}, // B
        {1, 26, 0}, // X
        {1, 27, 0}, // Y
    };



bool buttonInit(void)
{
  bool ret = true;

  for (int i=0; i<BUTTON_MAX_CH; i++)
  {
    if (button_tbl[i].pull_up)
    {   
      pinMode(button_tbl[i].pin, INPUT_PULLUP);
    }
    else
    {
      pinMode(button_tbl[i].pin, INPUT);
    }
  }


  return ret;
}

bool buttonGetPressed(uint8_t ch)
{
  bool ret = false;

  if (ch >= BUTTON_MAX_CH)
  {
    return false;
  }

  if (digitalRead(button_tbl[ch].pin) == button_tbl[ch].on_state)
  {
    ret = true;
  }

  return ret;
}

uint16_t buttonGetData(void)
{
  uint16_t ret = 0;


  for (int i=0; i<BUTTON_MAX_CH; i++)
  {
    ret |= (buttonGetPressed(i)<<i);
  }

  return ret;
}

enum ButtonObjState
{
  BUTTON_OBJ_WAIT_FOR_RELEASED,
  BUTTON_OBJ_WAIT_FOR_PRESSED,
  BUTTON_OBJ_PRESSED,
  BUTTON_OBJ_REPEATED_START,
  BUTTON_OBJ_REPEATED,
};

void buttonObjCreate(button_obj_t *p_obj, uint8_t ch, uint32_t pressed_time, uint32_t repeat_start_time, uint32_t repeat_pressed_time)
{
  p_obj->ch = ch;
  p_obj->state = 0;
  p_obj->pre_time = millis();
  p_obj->pressed_time = pressed_time;
  p_obj->repeat_start_time = repeat_start_time;
  p_obj->repeat_pressed_time = repeat_pressed_time;
  p_obj->event_flag = 0;
  p_obj->state_flag = 0;
  p_obj->click_count = 0;
}

bool buttonObjUpdate(button_obj_t *p_obj)
{
  bool ret = false;


  switch(p_obj->state)
  {
    case BUTTON_OBJ_WAIT_FOR_RELEASED:
      if (buttonGetPressed(p_obj->ch) == false)
      {
        p_obj->state = BUTTON_OBJ_WAIT_FOR_PRESSED;
      }
      break;

    case BUTTON_OBJ_WAIT_FOR_PRESSED:
      if (buttonGetPressed(p_obj->ch) == true)
      {
        p_obj->state = BUTTON_OBJ_PRESSED;
        p_obj->pre_time = millis();        
        p_obj->click_count = 0;
      }
      break;

    case BUTTON_OBJ_PRESSED:
      if (buttonGetPressed(p_obj->ch) == true)
      {
        if (millis()-p_obj->pre_time >= p_obj->pressed_time)
        {
          ret = true; 
          p_obj->state = BUTTON_OBJ_REPEATED_START;
          p_obj->pre_time = millis();
          p_obj->event_flag |= BUTTON_EVT_CLICKED;
          
          p_obj->state_flag |= BUTTON_STATE_PRESSED;
          p_obj->click_count++;
        }
      }
      else
      {
        p_obj->state = BUTTON_OBJ_WAIT_FOR_PRESSED;
        
        if (p_obj->state_flag & BUTTON_STATE_PRESSED)
        {
          p_obj->event_flag |= BUTTON_EVT_RELEASED;

          p_obj->state_flag |= BUTTON_STATE_RELEASED;
          p_obj->state_flag &= ~BUTTON_STATE_PRESSED;
          p_obj->state_flag &= ~BUTTON_STATE_REPEATED;          
        }
      }
      break;

    case BUTTON_OBJ_REPEATED_START:
      if (buttonGetPressed(p_obj->ch) == true)
      {
        if (millis()-p_obj->pre_time >= p_obj->repeat_start_time)
        {
          ret = true;
          p_obj->pre_time = millis();

          ret = true; 
          p_obj->state = BUTTON_OBJ_REPEATED;

          p_obj->event_flag |= BUTTON_EVT_CLICKED;
          p_obj->event_flag |= BUTTON_EVT_REPEATED;

          p_obj->state_flag |= BUTTON_STATE_REPEATED;
          p_obj->click_count++;
        }
      }
      else
      {
        p_obj->state = BUTTON_OBJ_PRESSED;
        p_obj->pre_time = millis();
      }
      break;

    case BUTTON_OBJ_REPEATED:
      if (buttonGetPressed(p_obj->ch) == true)
      {
        if (millis()-p_obj->pre_time >= p_obj->repeat_pressed_time)
        {
          ret = true;
          p_obj->pre_time = millis();

          p_obj->event_flag |= BUTTON_EVT_CLICKED;
          p_obj->event_flag |= BUTTON_EVT_REPEATED;

          p_obj->state_flag |= BUTTON_STATE_REPEATED;
          p_obj->click_count++;
        }
      }
      else
      {
        p_obj->state = BUTTON_OBJ_PRESSED;
        p_obj->pre_time = millis();

      }
      break;
  }
  
  return ret;
}

uint8_t buttonObjGetEvent(button_obj_t *p_obj)
{
  return p_obj->event_flag;
}

void buttonObjClearEvent(button_obj_t *p_obj)
{
  p_obj->event_flag = 0;
}

uint8_t buttonObjGetState(button_obj_t *p_obj)
{
  return p_obj->state_flag;
}



static button_obj_t button_obj[BUTTON_MAX];    



Button::~Button()
{
    
}

bool Button::begin(void)
{
  bool ret;

  buttonInit();

  for (int i=0; i<BUTTON_MAX; i++)
  {
    buttonObjCreate(&button_obj[i], i, 50, 1000, 100);
  }


  is_init = true;
  return ret;
}

bool Button::isClicked(ButtonName_t btn_name)
{
  bool ret = false;

  if (buttonObjGetEvent(&button_obj[btn_name]) & BUTTON_EVT_CLICKED)
  {
    ret = true;
    button_obj[btn_name].event_flag &= ~BUTTON_EVT_CLICKED;
  }

  return ret;
}

bool Button::isPressed(ButtonName_t btn_name)
{
  bool ret = false;

  ret = buttonGetPressed(btn_name);

  return ret;
}

uint32_t Button::getData(void)
{
  return buttonGetData();
}

void Button::clearEvent(void)
{
  for (int i=0; i<BUTTON_MAX; i++)
  {  
    buttonObjClearEvent(&button_obj[i]);
  }    
}

void Button::clearEvent(ButtonName_t btn_name)
{
  buttonObjClearEvent(&button_obj[btn_name]);
}

bool Button::update(void)
{
  for (int i=0; i<BUTTON_MAX; i++)
  {  
    buttonObjUpdate(&button_obj[i]);
  }
  return true;
}
