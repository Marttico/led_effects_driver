#include "led_effects_driver.hpp"
#include "led_effects_driver_priv.hpp"

std::vector<led_animation_queue_entry> animation_queue;


void led_effects_add_to_queue(led_animation_queue_entry entry){
    animation_queue.push_back(entry);
}

void led_effects_remove_from_queue(){

}


// Process each queued animation. Make sure you delete the ones that are finished.
void process_led_effects_queue(led_pixel_t** strip,uint32_t timetick){

    for(led_animation_queue_entry d : animation_queue){
        
    }

}