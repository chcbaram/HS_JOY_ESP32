#include <HS_JOY_ESP32.h>



LVGL_IMG_DEF(lvgl_img);



void setup()
{
  hs_joy.begin(115200);
  hs_joy.menu.add("배터리", NULL, batteryInfo);
  hs_joy.menu.add("버튼", NULL, buttonInfo);
  hs_joy.menu.add("조이스틱", NULL, joystickInfo);
  hs_joy.menu.add("Buzzer", NULL, buzzerInfo);
  hs_joy.menu.add("Image", NULL, imageInfo);
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

void buzzerInfo()
{
  int melody[] = {
    NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
  };
  int noteDurations[] = {
    4, 8, 8, 4, 4, 4, 4, 4
  };


  while(1)
  {
    hs_joy.lcd.clearDisplay();
    hs_joy.lcd.printf(0, 0, "Buzzer");

    hs_joy.lcd.printf(0,32, "Press A Key");              
    hs_joy.lcd.display();


    if (hs_joy.button.isClicked(BUTTON_A))
    {
      for (int thisNote = 0; thisNote < 8; thisNote++) 
      {
        int noteDuration = 1000 / noteDurations[thisNote];
        hs_joy.buzzer.playTone(melody[thisNote], noteDuration);

        int pauseBetweenNotes = noteDuration * 1.30;
        delay(pauseBetweenNotes);
        hs_joy.buzzer.stopTone();
      }
    }

    if (hs_joy.button.isClicked(BUTTON_START))
    {
      hs_joy.menu.exit();
      break;
    }
  }
}

void imageInfo()
{
  int16_t img_x;
  int16_t img_y;
  int16_t img_w;
  int16_t img_h;
  int16_t img_step;

  image_t img;

  img = hs_joy.lcd.createImage(lvgl_img);


  img_x = 0;
  img_y = 100;
  img_w = 100;
  img_h = 40;
  img_step = 10;

  while(1)
  {
    hs_joy.lcd.clearDisplay();

    if (hs_joy.button.isPressed(BUTTON_LEFT))
    {
      if (img_x > 0) img_x -= img_step;
    }
    if (hs_joy.button.isPressed(BUTTON_RIGHT))
    {
      if (img_x < (img.w-img_w)) img_x += img_step;
    }
    if (hs_joy.button.isPressed(BUTTON_UP))
    {
      if (img_y > 0) img_y -= img_step;
    }
    if (hs_joy.button.isPressed(BUTTON_DOWN))
    {
      if (img_y < (img.h-img_h)) img_y += img_step;
    }

    hs_joy.lcd.drawImage(img, (LCD_WIDTH-img_w)/2, (LCD_HEIGHT-img_h)/2, img_x, img_y, img_w, img_h);
    hs_joy.lcd.drawRect((LCD_WIDTH-img_w)/2, (LCD_HEIGHT-img_h)/2, img_w, img_h, WHITE);

    hs_joy.lcd.display();


    if (hs_joy.button.isClicked(BUTTON_START))
    {
      hs_joy.menu.exit();
      break;
    }
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
