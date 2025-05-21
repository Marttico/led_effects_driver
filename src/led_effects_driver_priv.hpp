#include "led_strip.h"
#include "freertos/task.h"

led_strip_handle_t led_strip = NULL;
uint32_t led_amount;
uint8_t brightness;
uint8_t fps;
TaskHandle_t xLedTaskHandle = NULL;

struct led_pixel_t {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

led_pixel_t** led_strip_arr;

void led_manager_trigger_handler(void* arg);