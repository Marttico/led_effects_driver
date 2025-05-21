#include "led_strip.h"
#include "freertos/task.h"
#include <vector>

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

enum led_animation_t{
    LED_ANIMATION_RIPPLE,
    LED_ANIMATION_HEARTBEAT,
    LED_ANIMATION_WIPE,
    LED_ANIMATION_RAINBOW,
    LED_ANIMATION_SINGLE_FLASH
};

class led_animation_queue_entry {
    private:
        uint8_t red, green, blue;
        uint32_t time_length;
        enum led_animation_t animation_type;
    public:
        led_animation_queue_entry(uint32_t time_length, enum led_animation_t anim_type);
        led_animation_queue_entry(uint8_t r, uint8_t g, uint8_t b, uint32_t t_length, enum led_animation_t anim_type);
        void led_animation_process(led_pixel_t** strip, uint32_t timetick);

};


void led_manager_trigger_handler(void* arg);