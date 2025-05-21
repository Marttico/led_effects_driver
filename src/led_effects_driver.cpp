#include "led_effects_driver.hpp"
#include "led_effects_driver_priv.hpp"

#define TAG "led_effects_driver"

#define ReturnOnUninitialized() if(led_strip == NULL){ESP_LOGE(TAG,"led_effects_driver has not been initialized yet!");return ESP_FAIL;}

esp_err_t init_led_effects_driver(int gpio_pin,int led_amt,int input_fps){
    ESP_LOGI(TAG,"Initializing led_effects_driver");

    if (input_fps < 12 || input_fps > 60){
        ESP_LOGE(TAG,"Configured FPS out of bounds (12 FPS <---> 60 FPS)");
        return ESP_FAIL;
    } 

    fps = input_fps;

    if (led_amt <= 0){
        ESP_LOGE(TAG,"Configured LED amount out of bounds (0 <---> ..)");
        return ESP_FAIL;
    }

    led_amount = led_amt;

    led_strip_config_t strip_config = {
        .strip_gpio_num = gpio_pin,
        .max_leds = (uint32_t) led_amt,
        .led_model = LED_MODEL_WS2812,
        .flags = {
            .invert_out = false,
        }
    };

    led_strip_rmt_config_t rmt_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 10 * 1000 * 1000,
        .mem_block_symbols = 0, // Let the driver choose
        .flags = {
            .with_dma = false,
        }
    };

    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));

    led_strip_arr = (led_pixel_t**)malloc(sizeof(led_pixel_t*)*led_amount);

    for(int index = 0; index < led_amount-1; index++){
        led_strip_arr[index] = (led_pixel_t*) malloc(sizeof(led_pixel_t));
    }

    xTaskCreate(led_manager_trigger_handler, "LED_MANAGER_TASK", 2048, NULL, NULL, &xLedTaskHandle);

    return ESP_OK;
}

esp_err_t deinit_led_effects_driver(){
    ReturnOnUninitialized();
    for(int index = 0; index < led_amount-1; index++){
        free(led_strip_arr[index]);
    }

    free(led_strip_arr);
    return ESP_OK;
}

esp_err_t trigger_animation(){
    ReturnOnUninitialized();

    ESP_LOGI(TAG,"Triggered Animation");
    return ESP_OK;
}



void led_manager_trigger_handler(void* arg){
    uint64_t ticknumber = 0;

    TickType_t xLastWakeTime;
    BaseType_t xWasDelayed;
    
    xLastWakeTime = xTaskGetTickCount ();

    for(;;){

        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(1000/fps));
        xLastWakeTime = xTaskGetTickCount();
        
        // SIMPLE LED TEST, DO NOT USE
        for(int i = 0; i < led_amount; i++){
            int currentLED = (ticknumber%led_amount) == i;
            led_strip_set_pixel(led_strip, i, currentLED*255, currentLED*255, currentLED*255);
        }
        led_strip_refresh(led_strip);


        ticknumber++;

    }
}