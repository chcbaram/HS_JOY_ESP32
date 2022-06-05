#include <HS_JOY_ESP32.h>




void setup() 
{
  hs_joy.begin(115200);
  hs_joy.menu.add("배터리", NULL, batteryInfo);
  hs_joy.menu.add("버튼", NULL, buttonInfo);
  hs_joy.menu.add("조이스틱", NULL, joystickInfo);
  hs_joy.menu.add("정보", NULL, info);
}


void loop() 
{
  static uint32_t pre_time;

  if (millis()-pre_time >= 500)
  {
    pre_time = millis();
    hs_joy.led.toggle();
  }
  hs_joy.menu.update();
}

void batteryInfo() 
{
  // put your main code here, to run repeatedly

  hs_joy.lcd.clearDisplay();
  hs_joy.lcd.printf(0, 0, "Battery Voltage");

  hs_joy.lcd.printf(0, 16*1,"%1.2f V", hs_joy.battery.getVoltage());
  hs_joy.lcd.printf(0, 16*2,"%3d %%", hs_joy.battery.getLevel());
  hs_joy.lcd.display();
  
  if (hs_joy.button.isClicked(BUTTON_START))
  {
    hs_joy.menu.exit();
  }
}

void buttonInfo() 
{  
  static uint32_t pre_time = millis();

  if (millis() - pre_time >= 30)
  {
    pre_time = millis();
    
    hs_joy.lcd.clearDisplay();
    hs_joy.lcd.printf(0, 0, "버튼 정보");

    for (int i=0; i<BUTTON_MAX; i++)
    {
      hs_joy.lcd.printf(8*i, 16,"%d", hs_joy.button.isPressed(i));
    }
      
    hs_joy.lcd.display();
  }
  
  if (hs_joy.button.isClicked(BUTTON_START))
  {
    hs_joy.menu.exit();
  }
}

void joystickInfo() 
{  
  static uint32_t pre_time = millis();
  static uint8_t mode = 0;

  if (millis() - pre_time >= 30)
  {
    pre_time = millis();
    
    hs_joy.lcd.clearDisplay();
    
    if (mode == 0)
    {
      hs_joy.lcd.printf(0, 16*0, "JoyStick Left");
      
      hs_joy.lcd.printf(0, 16*1,"       X :    Y", hs_joy.stickL.getAdcX(), hs_joy.stickL.getAdcY());
      hs_joy.lcd.printf(0, 16*2,"Adc %4d : %4d", hs_joy.stickL.getAdcX(), hs_joy.stickL.getAdcY());
      hs_joy.lcd.printf(0, 16*3,"Val %4d : %4d", hs_joy.stickL.getX(), hs_joy.stickL.getY());      
    }
    else
    {
      hs_joy.lcd.printf(0, 0, "JoyStick Right");
      hs_joy.lcd.printf(0, 16*1,"       X :    Y", hs_joy.stickR.getAdcX(), hs_joy.stickR.getAdcY());
      hs_joy.lcd.printf(0, 16*2,"Adc %4d : %4d", hs_joy.stickR.getAdcX(), hs_joy.stickR.getAdcY());
      hs_joy.lcd.printf(0, 16*3,"Val %4d : %4d", hs_joy.stickR.getX(), hs_joy.stickR.getY());   
    }
      
    hs_joy.lcd.display();
  }
  
  if (hs_joy.button.isClicked(BUTTON_A))
  {
    mode ^= 1;
  }
  if (hs_joy.button.isClicked(BUTTON_START))
  {
    hs_joy.menu.exit();
  }
}

void info()
{
  hs_joy.lcd.clearDisplay();
  hs_joy.lcd.printf(0, 0, "HS JOY ESP32");
  hs_joy.lcd.printf(0,16, "V 2022.06.05");
  hs_joy.lcd.display();
  
  if (hs_joy.button.isClicked(BUTTON_START))
  {
    hs_joy.menu.exit();
  }
}