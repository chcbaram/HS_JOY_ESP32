/*
 * BLE Gamepad Example
 * 
 * Requires "ESP32-BLE-Gamepad" library.
 * Please install the library first before running
 * 
 */

#include <HS_JOY_ESP32.h>
#include <Arduino.h>
#include <BleGamepad.h>

// Create BLE Gamepad with device name "HS_JOY_ESP32"
BleGamepad bleGamepad("HS_JOY_ESP32");

void setup()
{
  // Class for button configuration
  BleGamepadConfiguration blegamepad_conf = BleGamepadConfiguration();
  
  // set the joystick(axis) to use
  // HS_JOY_ESP32 uses xAxis, yAxis(left joystick), rxAxis, rzAxis(right joystick)
  // Each parameter is as follows.
  // xAxis, yAxis, zAxis, rxAxis, ryAxis, rzAxis, slider1, slider2
  blegamepad_conf.setWhichAxes(true, true, true, false, false, true, false, false);

  // special buttons to use
  // HS_JOY_ESP32 only uses the Start button.
  // Each parameter is as follows.
  // Start, select, menu, home, back, volumeInc, volumeDec, volumeMute
  blegamepad_conf.setWhichSpecialButtons(true, false, false, false, false, false, false, false);

  // Number of buttons to use
  // Right button x, y, a, b total 4.
  blegamepad_conf.setButtonCount(4);
  
  // number of pads to use
  // Set one left direction button (up, down, left, right).
  blegamepad_conf.setHatSwitchCount(1); 

  // Start HS_JOY_ESP32 
  hs_joy.begin(115200);

  
  Serial.println("Starting BLE work!");   

  // Start the gamepad! It is activated with the buttons set in blegamepad_conf.
  bleGamepad.begin(&blegamepad_conf);    
}

// Get each joystick value
// This is to change the ADC value of the joystick from 0 to 4095 to -32767 to 32767.
int32_t get_joystick(uint8_t ch) {
  int32_t value;
  int32_t value_out;
  uint16_t adc_value;
  
  int32_t dead_zone = 200;
  int32_t adc_max_value = 1700; 
  int32_t adc_offset = 4095/2;
  
  if(ch > 3){
    return 0;
  }
  switch(ch){
    case 0:
    adc_value = hs_joy.stickL.getAdcX();
    break;
    case 1:
    adc_value = hs_joy.stickL.getAdcY();
    break;
    case 2:
    adc_value = hs_joy.stickR.getAdcX();
    break;
    case 3:
    adc_value = hs_joy.stickR.getAdcY();
    break;
  }
  
  value = -constrain(adc_value-adc_offset, -2000, 2000);
  if (value >  dead_zone)      value -= dead_zone;
  else if (value < -dead_zone) value += dead_zone;
  else                                value  = 0;

  value_out = constrain(value, -adc_max_value, adc_max_value);
  value   = map(value_out, -adc_max_value, adc_max_value, -32767, 32767);
  return value;
}


void loop()
{
  uint32_t pre_time = 0;
  // Only when a gamepad is connected...
  if (bleGamepad.isConnected())
  {      
    uint8_t pad = 0;
    // run every 200ms
    if(millis() - pre_time > 200) {      
      pre_time = millis();
      // Get the arrow value from the HS_JOY_ESP32 and convert it to PAD.
      // PAD value has a total of 9 states (including the center state).
      // "Left + Top", "Right + Top", "Left + Bottom", "Right + Bottom", "Top", "Bottom", "Left", "Right", "Center"
      if (hs_joy.button.isPressed(BUTTON_UP)) {         
         if(hs_joy.button.isPressed(BUTTON_LEFT)) {
          pad = HAT_UP_LEFT;
         } else if(hs_joy.button.isPressed(BUTTON_RIGHT)) {
          pad = HAT_UP_RIGHT;
         } else {
          pad = HAT_UP;
         }        
      } else if (hs_joy.button.isPressed(BUTTON_DOWN)) {        
         if(hs_joy.button.isPressed(BUTTON_LEFT)) {
          pad = HAT_DOWN_LEFT;
         } else if(hs_joy.button.isPressed(BUTTON_RIGHT)) {
          pad = HAT_DOWN_RIGHT;
         } else {
          pad = HAT_DOWN;
         }    
      } else if (hs_joy.button.isPressed(BUTTON_LEFT)) {  
          pad = HAT_LEFT;
      } else if (hs_joy.button.isPressed(BUTTON_RIGHT)) {  
          pad = HAT_RIGHT;
      } else {
        pad = HAT_CENTERED;
      }      

      // Transmit the PAD value.
      bleGamepad.setHat1(pad);    

      // Get each joystick value
      int32_t l_x = get_joystick(0);
      int32_t l_y = -get_joystick(1);
      int32_t r_x = get_joystick(2);
      int32_t r_y = -get_joystick(3);

      // Joystick state transfer
      // caution!! The rZ position is different from the setWhichAxes function, so you have to be careful when entering parameters.
      // Each parameter is as follows.
      // x, y, z, rZ, rX, rY, slider1, slider2
      bleGamepad.setAxes(l_x, l_y, r_x, r_y, 0, 0, 0, 0);

      // Transmit the state of each button(including special buttons)

      // START
      if (hs_joy.button.isPressed(BUTTON_START)) {
        bleGamepad.pressStart();
      } else {
        bleGamepad.releaseStart();
      }

      // X
      if (hs_joy.button.isPressed(BUTTON_X)) {
        bleGamepad.press(BUTTON_1);
      } else {
        bleGamepad.release(BUTTON_1);
      }

      // A
      if (hs_joy.button.isPressed(BUTTON_A)) {
        bleGamepad.press(BUTTON_2);
      } else {
        bleGamepad.release(BUTTON_2);
      }

      // B
      if (hs_joy.button.isPressed(BUTTON_B)) {
        bleGamepad.press(BUTTON_3);
      } else {
        bleGamepad.release(BUTTON_3);
      }

      // Y
      if (hs_joy.button.isPressed(BUTTON_Y)) {
        bleGamepad.press(BUTTON_4);
      } else {
        bleGamepad.release(BUTTON_4);
      }
    }
  }
}
