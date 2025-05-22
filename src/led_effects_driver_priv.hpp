
#include "freertos/task.h"
#include "led_strip.h"
#include <vector>
#include <string>


struct led_pixel_t {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};


void led_manager_trigger_handler(void* arg);


void process_led_effects_queue(led_pixel_t** strip,uint64_t timetick);

// enum led_animation_t{
//     LED_ANIMATION_RIPPLE,
//     LED_ANIMATION_HEARTBEAT,
//     LED_ANIMATION_WIPE,
//     LED_ANIMATION_RAINBOW,
//     LED_ANIMATION_SINGLE_FLASH
// };

class led_animation {
    private:
        std::string name;
        uint32_t time_length;
        uint32_t current_time;
        std::vector<led_animation> *animation_queue;

    public:
        led_animation(uint32_t t_l, std::vector<led_animation>* anim_q;){
            animation_queue = anim_q;
            time_length = t_l;
            current_time = 0;
        }
        
        std::string getName(){
            return name;
        }
        
        void removeFromQueue(){
            animation_queue;
        }

        virtual void processAnimation(led_pixel_t** strip, uint32_t timetick) = 0;


}

class led_ripple : public led_animation {
    private:
        uint8_t red, green, blue;

    public:
    led_ripple(uint32_t t_l, std::string nm, uint8_t r, uint8_t g, uint8_t b) : led_animation(t_l, nm){
        red = r;
        green = g;
        blue = b;
    }

    void processAnimation(led_pixel_t** strip, uint32_t timetick){

    }

}


// class led_animation_queue_entry {
//     private:
//         uint8_t red, green, blue;
//         uint32_t time_length;
//         led_animation_t animation_type;
//     public:
//         led_animation_queue_entry(uint32_t time_length, enum led_animation_t anim_type);
//         led_animation_queue_entry(uint8_t r, uint8_t g, uint8_t b, uint32_t t_length, enum led_animation_t anim_type);
//         void led_animation_process(led_pixel_t** strip, uint32_t timetick);

// };
