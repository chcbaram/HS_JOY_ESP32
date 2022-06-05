/*
 * menu.h
 *
 *      Author: Baram
 */

#ifndef _MENU_H_
#define _MENU_H_

#include <Arduino.h>
#include "../def.h"


#define MENU_MAX_NODE             32


typedef struct
{
  uint8_t index;
  char    str[16];
  uint8_t str_length;

  void (*setup_func)(void);
  void (*loop_func)(void);
} menu_node_t;


typedef struct 
{
  uint8_t run_count;  
  uint8_t count;
  uint8_t cursor;
  uint8_t first_rows;
  uint8_t view_rows;
  uint32_t pre_time;
  uint32_t pre_time_pressed;
  uint32_t press_count;
  bool     press_state;
  menu_node_t node[MENU_MAX_NODE];
} menu_t;



class Menu
{
  public:
    Menu();
    ~Menu();
    
    bool begin(void);

    void add(const char *menu_str, void (*setup_func)(void) = NULL, void (*loop_func)(void) = NULL);
    void exit(void);
    void update(void);

  private:
    uint32_t pre_time[8];
    menu_t menu;
    menu_t *p_menu;

    bool drawMenu(menu_t *p_menu);
  
};


#endif