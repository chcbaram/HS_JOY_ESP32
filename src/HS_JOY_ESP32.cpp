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
 
  adcInfoInit();
  i2c_bus.begin(21, 22);

  led.begin();
  lcd.begin();
  button.begin();
  battery.begin();
  menu.begin();
  stickL.begin(ADC_CH_STICK_L_X, ADC_CH_STICK_L_Y);
  stickR.begin(ADC_CH_STICK_R_X, ADC_CH_STICK_R_Y);

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
  battery.update();

  return true;
}



HS_JOY_ESP32 hs_joy;
