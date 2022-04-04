// 
// The code in the link below has been converted for HG_JOY_ESP32 with Adafruit GFX.
// https://wokwi.com/projects/326927455693570643
#include <HS_JOY_ESP32.h>

const uint8_t PROGMEM img_bubble_tip[] = {
B01111100,     //░█████░░
B00111000,     //░░███░░░
B00010000      //░░░█░░░░
};

int potentiometer_value = 0;

int center_x = 64;
int center_y = 108;
int radius_pixel = 92;
int radius_line = 87;
int radius_text = 75;

float start_x = 0;
float start_y = 0;
float end_x = 0;
float end_y = 0;
float text_x = 0;
float text_y = 0;
float angle = 0;

int value_min = 0;
int value_max = 1000;

int tick_value = 0; // value for the current tickmark

byte precalculated_x_radius_pixel[150]; // lookup table to prevent expensive sin/cos calculations
byte precalculated_y_radius_pixel[150]; // lookup table to prevent expensive sin/cos calculations

byte precalculated_x_radius_line[150]; // lookup table to prevent expensive sin/cos calculations
byte precalculated_y_radius_line[150]; // lookup table to prevent expensive sin/cos calculations

byte precalculated_x_radius_text[150]; // lookup table to prevent expensive sin/cos calculations
byte precalculated_y_radius_text[150]; // lookup table to prevent expensive sin/cos calculations

float fps;  // FPS measurement for performance optimizations
unsigned long millis_time;       // fps
unsigned long millis_time_last;  // fps

char knob_value[20];    // big value on top of the screen
int string_width;

void setup() {    
  hs_joy.begin(115200);
  hs_joy.lcd.clearDisplay();
  hs_joy.lcd.display();
  
  // pre-calculate x and y positions for radius_pixel;
  for (int i = 0; i < 150; i++) {
      precalculated_x_radius_pixel[i] = round(-sin(radians(i-90))*radius_pixel + center_x);
      precalculated_y_radius_pixel[i] = round(-cos(radians(i-90))*radius_pixel + center_y);    

      precalculated_x_radius_line[i] = round(-sin(radians(i-90))*radius_line + center_x);
      precalculated_y_radius_line[i] = round(-cos(radians(i-90))*radius_line + center_y);   

      precalculated_x_radius_text[i] = round(-sin(radians(i-90))*radius_text + center_x);
      precalculated_y_radius_text[i] = round(-cos(radians(i-90))*radius_text + center_y);   
  }
}

uint16_t getStrWidth(char* text) {
    int16_t x1, y1;
    uint16_t w, h;
    hs_joy.lcd.getTextBounds(knob_value, 0, 0, &x1, &y1, &w, &h);
    return w - x1; 
}

void loop() {
    hs_joy.lcd.clearDisplay();
    hs_joy.lcd.setTextSize(1);      // Normal 1:1 pixel scale
    hs_joy.lcd.setTextColor(WHITE); // Draw white text 
    hs_joy.lcd.cp437(true);         // Use full 256 char 'Code Page 437' font 
    for (int i = -42; i <= 42; i=i+3) {
        angle = i + int((int(potentiometer_value+1)*3)%30/10); // potentiometer_value is multiplied by 10, 3° = 1 step     
    
        // get values from pre-calculated look-up table
        start_x = precalculated_x_radius_pixel[(uint8_t)round(angle+90)];
        start_y = precalculated_y_radius_pixel[(uint8_t)round(angle+90)]; 
        
        if (start_x > 0 && start_y > 0 && start_x < 128 && start_y < 64) { 
            tick_value = round((potentiometer_value/10.0) - angle/3.0); // get real value for the currently drawn tickmark
            if(tick_value >= (value_min/10) && tick_value <= (value_max/10)) {
                if ((int)tick_value % 10 == 0) {      
                    end_x = precalculated_x_radius_line[(uint8_t)round(angle+90)];
                    end_y = precalculated_y_radius_line[(uint8_t)round(angle+90)]; 
                    
                    hs_joy.lcd.drawLine(round(start_x), round(start_y), round(end_x), round(end_y), WHITE);
                                        
                    text_x = precalculated_x_radius_text[(uint8_t)round(angle+90)];
                    text_y = precalculated_y_radius_text[(uint8_t)round(angle+90)]; 
                
                    if (text_x > 0 && text_x < 128 && text_y < 64) {    // only draw text if it´s visible
                        snprintf(knob_value, sizeof(knob_value), "%d", (int)(tick_value)); // convert to c string
                        string_width = getStrWidth(knob_value);    // get string width in pixels                        
                        hs_joy.lcd.setCursor(text_x-string_width/2, text_y);     // Start at top-left corner  
                        hs_joy.lcd.print(knob_value);  // draw the text
                    }     
                    // end label drawing
                }
                else // this is a small tickmark, we don´t need to calculate end position, as only pixels are drawn in this case
                {
                    hs_joy.lcd.drawPixel(start_x, start_y, WHITE); // dots instead of lines  
                }
            }  // end draw only between 0-100
        }
    }
     
    dtostrf(potentiometer_value/10.0, 1, 1, knob_value);  // float to string, -- value, min. width, digits after decimal, buffer to store
    sprintf(knob_value, "%s%s", knob_value, "%"); // add some random ending character

    string_width = getStrWidth(knob_value);    
    if (string_width % 2 == 1) {string_width++;}   // it easier to position things when the width is even number

    hs_joy.lcd.drawRoundRect(64-(string_width+4)/2, 0, string_width+4, 11, 3, WHITE);
    
    hs_joy.lcd.drawBitmap(64-3, 11, img_bubble_tip, 8, 3, WHITE);    

    hs_joy.lcd.setCursor(64-string_width/2, 10-8);     // Start at top-left corner   
    hs_joy.lcd.print(knob_value);

    // FPS counter (optional, disable for final version)
    hs_joy.lcd.setCursor(0,0);
    hs_joy.lcd.print((int)fps);    
    hs_joy.lcd.display();
     
    uint16_t ain = analogRead(2)>>2;
    potentiometer_value = constrain(map(ain, 0, 1023, value_max, value_min), value_min, value_max);
  
    // FPS counter calculations
    millis_time_last = millis_time;
    millis_time = millis();
    fps = millis_time - millis_time_last;
    fps = round(1000.0 / fps*1.0);
}