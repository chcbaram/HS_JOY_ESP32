#include <HS_JOY_ESP32.h>




void setup() 
{
  hs_joy.begin(115200);
  hs_joy.menuAdd("배터리", NULL, batteryInfo);
  hs_joy.menuAdd("버튼", NULL, buttonInfo);
  hs_joy.menuAdd("정보", info, NULL);
}


void loop() 
{
  hs_joy.menuUpdate();
}

void info()
{
  hs_joy.lcd.clearDisplay();
  hs_joy.lcd.printf(0, 0, "HS JOY ESP32");
  hs_joy.lcd.printf(0,16, "V 2022.06.05");
  hs_joy.lcd.display();
  delay(2000);
}

void batteryInfo() 
{
  // put your main code here, to run repeatedly
  float vol;

  hs_joy.lcd.clearDisplay();
  hs_joy.lcd.printf(0, 0, "Battery Voltage");
  vol = hs_joy.batteryGetVoltage();
  hs_joy.lcd.printf(0, 16,"%1.1f V", vol/10);
  hs_joy.lcd.display();
  delay(100);
}


void buttonInfo() 
{  

  hs_joy.lcd.clearDisplay();
  hs_joy.lcd.printf(0, 0, "버튼 정보");

  for (int i=0; i<BUTTON_MAX; i++)
  {
    hs_joy.lcd.printf(8*i, 16,"%d", hs_joy.button.isPressed(i));
  }
    
  hs_joy.lcd.display();
  delay(100);
}
