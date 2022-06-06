#ifndef _OLED_H_
#define _OLED_H_

#include <Arduino.h>
#include "../def.h"
#include "../gfx/Adafruit_GFX_Library/Adafruit_GFX.h"
#include "../gfx/Adafruit_SSD1306/Adafruit_SSD1306.h"
#include "../hangul/PHan_Lib.h"
#include "lvgl/lvgl.h"



#define LCD_WIDTH   128
#define LCD_HEIGHT  64



enum class_color
{
 white     = 0xFFFF,
 gray      = 0x8410,
 darkgray  = 0xAD55,
 black     = 0x0000,
 purple    = 0x8010,
 pink      = 0xFE19,
 red       = 0xF800,
 orange    = 0xFD20,
 brown     = 0xA145,
 beige     = 0xF7BB,
 yellow    = 0xFFE0,
 lightgreen= 0x9772,
 green     = 0x07E0,
 darkblue  = 0x0011,
 blue      = 0x001F,
 lightblue = 0xAEDC,
};

#define LVGL_IMG_DEF(var_name) extern lvgl_img_t var_name;


typedef struct
{
  const lvgl_img_t *p_img;
  int16_t x;
  int16_t y;
  int16_t w;
  int16_t h;
} image_t;



class OLed : public Adafruit_SSD1306
{
  public:
    OLed(): Adafruit_SSD1306(LCD_WIDTH, LCD_HEIGHT, &i2c_bus, -1, 800000UL, 100000UL) { is_init = false; }
    ~OLed();
    
    boolean begin(void);
    void printf(int x, int y,  const char *fmt, ...);
    void disHanFont(int x, int y, PHAN_FONT_OBJ *FontPtr);
    bool isInit() { return is_init; };

    image_t createImage(lvgl_img_t &img, int16_t x=0, int16_t y=0, int16_t w=0, int16_t h=0);

    void drawImage(image_t &img, int16_t draw_x, int16_t draw_y, int16_t img_x=0, int16_t img_y=0, int16_t img_w=0, int16_t img_h=0);

  private:
    bool is_init;
};



#endif