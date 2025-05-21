#include "esp_err.h"
#include "esp_log.h"
#include "led_strip.h"
#include <sys/time.h>

esp_err_t init_led_effects_driver(int gpio_pin,int led_amt, int input_fps);

esp_err_t trigger_animation();


/*Idea of this led driver is to calculate all the brightnesses and colors of LEDs before actually applying them. 
  This way multiple animations can be shown at once.
  Using a queue queue and preset animations this can be done.
  If queue is empty, suspend task.

*/