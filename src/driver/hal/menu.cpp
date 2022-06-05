/*
 * menu.c
 *
 *  Created on: 2020. 12. 23.
 *      Author: baram
 */


#include "menu.h"
#include "../../HS_JOY_ESP32.h"



Menu::Menu()
{
  p_menu = &menu;
}

Menu::~Menu()
{
    
}

bool Menu::begin(void)
{
  p_menu->run_count = 0;
  p_menu->count = 0;
  p_menu->cursor = 0;
  p_menu->first_rows = 0;
  p_menu->view_rows = 4;
  p_menu->pre_time = millis();
  p_menu->press_count = 0;

  return true;
}

void Menu::add(const char *menu_str, void (*setup_func)(void), void (*loop_func)(void))
{
  uint8_t index;

  index = p_menu->count++;

  strcpy(p_menu->node[index].str, menu_str);
  p_menu->node[index].setup_func = setup_func;
  p_menu->node[index].loop_func = loop_func;
}

void Menu::update(void)
{
  if (p_menu->run_count > 0)
  {
    if (p_menu->run_count == 1)
    {
      p_menu->run_count++;
      if (p_menu->node[p_menu->cursor].setup_func != NULL)
      {
        p_menu->node[p_menu->cursor].setup_func();
      }
    }
    else
    {
      if (p_menu->node[p_menu->cursor].loop_func != NULL)
      {
        p_menu->node[p_menu->cursor].loop_func();        
      }
      else
      {
        p_menu->run_count = 0;
        p_menu->pre_time = millis();
        p_menu->press_count = 0;
      }
    }
  }
  else
  {
    drawMenu(p_menu);
  }  
}

void Menu::exit(void)
{
  p_menu->run_count = 0;
  p_menu->pre_time = millis();
  p_menu->press_count = 0;  

  hs_joy.button.clearEvent();
}

bool Menu::drawMenu(menu_t *p_menu)
{
  uint8_t view_rows;
  uint8_t index;
  uint8_t press_count = 0;
  bool press_done = false;


  if (p_menu->count == 0)
  {
    return false;
  }     

  // 버튼 처리 
  if (hs_joy.button.isClicked(BUTTON_DOWN))
  {
    p_menu->press_count = 1;
    press_count = p_menu->press_count;
    press_done = true;
  }

  if (hs_joy.button.isClicked(BUTTON_UP))
  {
    p_menu->press_count = 2;
    press_count = p_menu->press_count;
    press_done = true;
  }

  if (hs_joy.button.isClicked(BUTTON_A))
  {
    p_menu->press_count = 0;

    for (int i=0; i< hs_joy.lcd.width(); i+=8)
    {
      hs_joy.lcd.fillRect(i, 0, 8, hs_joy.lcd.height(), 0);
      hs_joy.lcd.display();
    }
    p_menu->run_count = 1;      
    return true;
  }

  // 커서 처리 
  if (press_done)
  {
    if (press_count == 1)
    {
      p_menu->cursor++;
      p_menu->cursor %= p_menu->count;            
    }
    if (press_count == 2)
    {
      p_menu->cursor--;
      p_menu->cursor = constrain(p_menu->cursor, 0, p_menu->count - 1);
    }
  }

  p_menu->first_rows = (p_menu->cursor/p_menu->view_rows) * p_menu->view_rows;


  view_rows = p_menu->count - p_menu->first_rows;
  view_rows = constrain(view_rows, 0, p_menu->view_rows);

  
  hs_joy.lcd.clearDisplay();

  for (int i=0; i<view_rows; i++)
  {
    index = p_menu->first_rows + i;
    hs_joy.lcd.printf(2, 16*i + 1, "%02d %s", index+1, p_menu->node[index].str);

    if (index == p_menu->cursor)
    {
      hs_joy.lcd.drawRoundRect(0, 16*i, hs_joy.lcd.width(), 16, 2, 1);
    }
  }
  hs_joy.lcd.display();

  return false;
}
