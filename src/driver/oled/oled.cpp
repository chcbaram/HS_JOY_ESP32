#include "oled.h"




  
OLed::~OLed()
{
    
}

boolean OLed::begin(void)
{
  boolean ret;
    
  ret = Adafruit_SSD1306::begin(SSD1306_SWITCHCAPVCC, 0x3C, false, false);

  textcolor = WHITE;
  textbgcolor = BLACK;

  clearDisplay(); 
  

  is_init = true;
  return ret;
}

void OLed::printf(int x, int y,  const char *fmt, ...)
{
  int32_t ret = 0;
  va_list arg;
  va_start (arg, fmt);
  int32_t len;
  char print_buffer[256];
  int Str_Size;
  int Size_Char;
  int i, x_Pre = x;
  PHAN_FONT_OBJ FontBuf;


  len = vsnprintf(print_buffer, 255, fmt, arg);
  va_end (arg);

  for( i=0; i<len; i+=Size_Char )
  {
    PHan_FontLoad( &print_buffer[i], &FontBuf );
      
      
    disHanFont( x, y, &FontBuf );
                      
    Size_Char = FontBuf.Size_Char;
    if (Size_Char >= 2)
    {           
        x += 2*8;
    }       
    else
    {
        x += 1*8;
    }
    
    if( 128 < x ) 
    {
        x  = x_Pre;
        y += 16;
    }
    
    if( FontBuf.Code_Type == PHAN_END_CODE ) break;
  }         
}

void OLed::disHanFont(int x, int y, PHAN_FONT_OBJ *FontPtr)
{
  uint16_t    i, j, Loop;
  uint16_t  FontSize = FontPtr->Size_Char;
  uint16_t index_x;

  if (FontSize > 2)
  {
    FontSize = 2;
  }

  for ( i = 0 ; i < 16 ; i++ )        // 16 Lines per Font/Char
  {
    index_x = 0;
    for ( j = 0 ; j < FontSize ; j++ )      // 16 x 16 (2 Bytes)
    {
      for( Loop=0; Loop<8; Loop++ )
      {
        if( FontPtr->FontBuffer[i*FontSize +j] & (0x80>>Loop))
        {
          drawPixel(x + index_x++, y + i, textcolor);
        } 
        else
        {
          drawPixel(x + index_x++, y + i, textbgcolor);
        }
      }
    }
  }
}

image_t OLed::createImage(lvgl_img_t &img, int16_t x, int16_t y, int16_t w, int16_t h)
{
  image_t ret;

  ret.x = x;
  ret.y = y;

  if (w > 0) ret.w = w;
  else       ret.w = img.header.w;

  if (h > 0) ret.h = h;
  else       ret.h = img.header.h;

  ret.p_img = &img;

  return ret;
}

void OLed::drawImage(image_t &img, int16_t draw_x, int16_t draw_y, int16_t img_x, int16_t img_y, int16_t img_w, int16_t img_h)
{
  int16_t o_x;
  int16_t o_y;  
  int16_t o_w;
  int16_t o_h;
  uint16_t *p_data;
  uint16_t pixel;


  o_w = img.w;
  o_h = img.h;

  if (img_w > 0) o_w = img_w;
  if (img_h > 0) o_h = img_h;

  p_data = (uint16_t *)img.p_img->data;

  for (int yi=0; yi<o_h; yi++)
  {
    o_y = (img.y + yi + img_y);
    if (o_y >= img.p_img->header.h) break;

    o_y = o_y * img.p_img->header.w;
    for (int xi=0; xi<o_w; xi++)
    {
      o_x = img.x + xi + img_x;
      if (o_x >= img.p_img->header.w) break;

      pixel = p_data[o_y + o_x];
      if (pixel != green)
      {
        if (pixel > 0)
          drawPixel(draw_x+xi, draw_y+yi, textcolor);
        else
          drawPixel(draw_x+xi, draw_y+yi, textbgcolor);
      }
    }
  }  
}