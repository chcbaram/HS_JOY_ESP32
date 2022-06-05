/*
 * stick.c
 *
 *  Created on: 2020. 12. 23.
 *      Author: baram
 */


#include "Buzzer.h"
#include "esp32-hal-ledc.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"



#define SOUND_PWM_CHANNEL   0
#define SOUND_RESOLUTION    8 // 8 bit resolution
#define SOUND_ON            (1<<(SOUND_RESOLUTION-1)) // 50% duty cycle
#define SOUND_OFF           0                         // 0% duty cycle


static TaskHandle_t _tone_task = NULL;
static QueueHandle_t _tone_queue = NULL;
static uint8_t _channel = 0;


typedef enum{
  TONE_START,
  TONE_END,
  TONE_SET_CHANNEL
} tone_cmd_t;

typedef struct{
  tone_cmd_t tone_cmd;
  uint8_t pin;
  unsigned int frequency;
  unsigned long duration;
  uint8_t channel;
} tone_msg_t;

static void tone_task(void*){
  tone_msg_t tone_msg;
  while(1){
    xQueueReceive(_tone_queue, &tone_msg, portMAX_DELAY);
    switch(tone_msg.tone_cmd){
      case TONE_START:
        log_d("Task received from queue TONE_START: _pin=%d, frequency=%u Hz, duration=%lu ms", tone_msg.pin, tone_msg.frequency, tone_msg.duration);
        log_d("Setup LED controll on channel %d", _channel);

        ledcSetup(_channel, tone_msg.frequency, SOUND_RESOLUTION);
        ledcAttachPin(tone_msg.pin, _channel);
        ledcWrite(_channel, SOUND_ON);

        if(tone_msg.duration)
        {
          delay(tone_msg.duration);
          ledcWrite(_channel, SOUND_OFF);          
          ledcDetachPin(tone_msg.pin);
        }
        break;

      case TONE_END:
        log_d("Task received from queue TONE_END: pin=%d", tone_msg.pin);
        ledcWrite(_channel, SOUND_OFF);          
        ledcDetachPin(tone_msg.pin);        
        break;

      case TONE_SET_CHANNEL:
        log_d("Task received from queue TONE_SET_CHANNEL: channel=%d", tone_msg.channel);
        _channel = tone_msg.channel;
        break;

      default: ; // do nothing
    } // switch
  } // infinite loop
}

static int tone_init(){
  if(_tone_queue == NULL){
    log_v("Creating tone queue");
    _tone_queue = xQueueCreate(128, sizeof(tone_msg_t));
    if(_tone_queue == NULL){
      log_e("Could not create tone queue");
      return 0; // ERR
    }
    log_v("Tone queue created");
  }

  if(_tone_task == NULL){
    log_v("Creating tone task");
    xTaskCreate(
      tone_task, // Function to implement the task
      "toneTask", // Name of the task
      3500,  // Stack size in words
      NULL,  // Task input parameter
      1,  // Priority of the task
      &_tone_task  // Task handle.
      );
    if(_tone_task == NULL){
      log_e("Could not create tone task");
      return 0; // ERR
    }
    log_v("Tone task created");
  }
  return 1; // OK
}

static void esp_setToneChannel(uint8_t channel){
  log_d("channel=%d", channel);
  if(tone_init()){
    tone_msg_t tone_msg = {
      .tone_cmd = TONE_SET_CHANNEL,
      .pin = 0, // Ignored
      .frequency = 0, // Ignored
      .duration = 0, // Ignored
      .channel = channel
    };
    xQueueSend(_tone_queue, &tone_msg, portMAX_DELAY);
  }
}

static void esp_noTone(uint8_t _pin){
  log_d("noTone was called");
  if(tone_init()){
    tone_msg_t tone_msg = {
      .tone_cmd = TONE_END,
      .pin = _pin,
      .frequency = 0, // Ignored
      .duration = 0, // Ignored
      .channel = 0 // Ignored
    };
    xQueueSend(_tone_queue, &tone_msg, portMAX_DELAY);
  }
}

// parameters:
// _pin - pin number which will output the signal
// frequency - PWM frequency in Hz
// duration - time in ms - how long will the signal be outputted.
//   If not provided, or 0 you must manually call noTone to end output
static void esp_tone(uint8_t _pin, unsigned int frequency, unsigned long duration){
  log_d("_pin=%d, frequency=%u Hz, duration=%lu ms", _pin, frequency, duration);
  if(tone_init()){
    tone_msg_t tone_msg = {
      .tone_cmd = TONE_START,
      .pin = _pin,
      .frequency = frequency,
      .duration = duration,
      .channel = 0 // Ignored
    };
    xQueueSend(_tone_queue, &tone_msg, portMAX_DELAY);
  }
}


Buzzer::Buzzer()
{
  out_pin = 15;
}

Buzzer::~Buzzer()
{
    
}

bool Buzzer::begin()
{ 
  return true;
}

void Buzzer::playTone(uint32_t freq, uint32_t duration_ms)
{
  esp_tone(out_pin, freq, duration_ms);
}

void Buzzer::stopTone()
{
  esp_noTone(out_pin);
}