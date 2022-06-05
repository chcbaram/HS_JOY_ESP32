/*
 * HS_JOY_ESP32.h
 *
 *  Created on: May 16, 2018
 *      Author: Baram
 */

#ifndef HS_JOY_ESP32_H_
#define HS_JOY_ESP32_H_

#include <Arduino.h>

#include "./driver/driver.h"





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



class HS_JOY_ESP32
{
  public:
    HS_JOY_ESP32();
    ~HS_JOY_ESP32();
        

    Led       led;
    OLed      lcd;
    Button    button;


    bool begin(int baud);    
    float batteryGetVoltage(void);

    void menuAdd(const char *menu_str, void (*setup_func)(void) = NULL, void (*loop_func)(void) = NULL);
    void menuExit(void);
    void menuUpdate(void);

    bool update(void);
    
  private:    
    bool printInitLog(const char *str_msg, bool ret);
    bool menuDraw(menu_t *p_menu);

    bool is_init;

    uint32_t pre_time[8];
    menu_t menu;
    menu_t *p_menu;
};

extern HS_JOY_ESP32 hs_joy;

#endif 
