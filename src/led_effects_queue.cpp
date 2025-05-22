#include "led_effects_driver.hpp"
#include "led_effects_driver_priv.hpp"

#define TAG "led_effects_queue"

std::vector<led_animation> animation_queue;


void led_effects_add_to_queue(led_animation entry){
    animation_queue.push_back(entry);
}

void led_effects_remove_from_queue(){

}


// Process each queued animation. Make sure you delete the ones that are finished.
void process_led_effects_queue(led_pixel_t** strip,uint64_t timetick){

    for(led_animation anim : animation_queue){
        

        ESP_LOGI(TAG,"%s",anim.getName().c_str());
    }

}