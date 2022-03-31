/*
 * HS_JOY_ESP32.cpp
 *
 *  Created on: Jul 6, 2018
 *      Author: Baram
 */



#include "HS_JOY_ESP32.h"
#include "image/hs_joy_img.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"





void taskUpdate( void *pvParameters );

void taskUpdate(void *pvParameters) 
{
  HS_JOY_ESP32 *p_class =  (HS_JOY_ESP32 *)pvParameters;
  
  for (;;)
  {
    p_class->update();
    vTaskDelay(2);
  }
}

HS_JOY_ESP32::HS_JOY_ESP32(void)
{
  p_menu = &menu; 
  is_init = false;
}

HS_JOY_ESP32::~HS_JOY_ESP32()
{ 
}

bool HS_JOY_ESP32::begin(int baud)
{
  bool ret = false;
  bool ret_log = false;

  if (is_init == true)
  {
    return true;
  }

  driverInit();

  Serial.begin(baud);
  Serial.println();
  Serial.println();
  Serial.println("[HS_JOY_ESP32]");

  pinMode(35, OUTPUT);
  
  digitalWrite(35, HIGH);

  
  adcInfoInit();
  adcInfoEnable(VBAT);
  
  lcd.begin();
  button.begin();

  for (int i=0; i<8; i++)
  {
    lcd.clearDisplay();
    lcd.drawBitmap((128-30)/2, (64-32)/2, &hs_joy_img[(7-i)*32*32/8], 32, 32, 1);

    if (i > 3)
    {
      lcd.setCursor(28, 64 - 8);
      lcd.print("HS_JOY_ESP32");
    }
    lcd.display();
    delay(80);
  }
  delay(500);
  lcd.clearDisplay();
  
  lcd.setCursor(0, 0);

  ret_log = true;

  if (ret_log == false)
  {
    lcd.display();
    lcd.setCursor(0, 0);
    delay(1000);
  }
  else
  {
    lcd.clearDisplay();
    lcd.setCursor(0, 0);    
  }

  for (int i=0; i<8; i++)
  {
    pre_time[i] = millis();
  }

  xTaskCreatePinnedToCore(
    taskUpdate
    ,  "update"
    ,  4*1024   // Stack size
    ,  (void *)this
    ,  1        // Priority
    ,  NULL 
    ,  1);
    

  p_menu->run_count = 0;
  p_menu->count = 0;
  p_menu->cursor = 0;
  p_menu->first_rows = 0;
  p_menu->view_rows = 4;
  p_menu->pre_time = millis();
  p_menu->press_count = 0;

  // disable brownout detector
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  is_init = true;
  return true;
}

bool HS_JOY_ESP32::printInitLog(const char *str_msg, bool ret)
{
  if (ret == true)
  {
    Serial.print("[OK] ");
    if (lcd.isInit())
    {
      lcd.print("[OK] ");
    }
  }
  else
  {
    Serial.print("[NG] ");
    if (lcd.isInit())
    {
      lcd.print("[NG] ");
    }
  }
  
  Serial.println(str_msg);
  if (lcd.isInit())
  {
    lcd.println(str_msg);
  }

  return ret;
}

bool HS_JOY_ESP32::update(void)
{
  uint32_t cur_time;

  cur_time = millis();

  if (cur_time-pre_time[0] >= 10)
  {
    pre_time[0] = cur_time;      
  }

  if (cur_time-pre_time[1] >= 50)
  {
    pre_time[1] = cur_time;
  }

  adcInfoUpdate();
  button.update();

  return true;
}

void HS_JOY_ESP32::ledOn(void)
{
  digitalWrite(32, LOW);
}

void HS_JOY_ESP32::ledOff(void)
{
  digitalWrite(32, HIGH);
}

void HS_JOY_ESP32::ledToggle(void)
{
  digitalWrite(32, !digitalRead(32));
}

uint8_t HS_JOY_ESP32::batteryGetVoltage(void)
{
  uint16_t value;

  value = adcInfoReadRaw(VBAT);

  value = 72 * value / 4095;  // 3.6V * x / 4095

  return value;
}

void HS_JOY_ESP32::menuAdd(const char *menu_str, void (*setup_func)(void), void (*loop_func)(void))
{
  uint8_t index;

  index = p_menu->count++;

  strcpy(p_menu->node[index].str, menu_str);
  p_menu->node[index].setup_func = setup_func;
  p_menu->node[index].loop_func = loop_func;
}

void HS_JOY_ESP32::menuUpdate(void)
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
    menuDraw(p_menu);
  }  
}

bool HS_JOY_ESP32::menuDraw(menu_t *p_menu)
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
  if (button.isClicked(BUTTON_DOWN))
  {
    p_menu->press_count = 1;
    press_count = p_menu->press_count;
    press_done = true;
  }

  if (button.isClicked(BUTTON_UP))
  {
    p_menu->press_count = 2;
    press_count = p_menu->press_count;
    press_done = true;
  }

  if (button.isClicked(BUTTON_START))
  {
    p_menu->press_count = 0;

    for (int i=0; i< lcd.width(); i+=4)
    {
      lcd.fillRect(i, 0, 4, lcd.height(), 0);
      lcd.display();
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

  
  lcd.clearDisplay();

  for (int i=0; i<view_rows; i++)
  {
    index = p_menu->first_rows + i;
    lcd.printf(2, 16*i + 1, "%02d %s", index+1, p_menu->node[index].str);

    if (index == p_menu->cursor)
    {
      lcd.drawRoundRect(0, 16*i, lcd.width(), 16, 2, 1);
    }
  }
  lcd.display();

  return false;
}


HS_JOY_ESP32 hs_joy;