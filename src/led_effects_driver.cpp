#include "led_effects_driver.hpp"
#include "led_effects_driver_priv.hpp"

#define TAG "led_effects_driver"

uint32_t led_amount;
uint8_t brightness;
uint8_t fps;
TaskHandle_t xLedTaskHandle = NULL;

std::vector<led_pixel_t> led_vector;
std::vector<std::unique_ptr<led_animation>> animation_queue;

led_strip_handle_t led_strip_handle = NULL;

#define ReturnOnUninitialized()                                                \
    if (led_strip_handle == NULL) {                                            \
        ESP_LOGE(TAG, "led_effects_driver has not been initialized yet!");     \
        return ESP_FAIL;                                                       \
    }

esp_err_t init_led_effects_driver(int gpio_pin, int led_amt) {
    return init_led_effects_driver(gpio_pin, led_amt, 255);
}

esp_err_t init_led_effects_driver(int gpio_pin, int led_amt, uint8_t b) {
    ESP_LOGI(TAG, "Initializing led_effects_driver");

    fps = 50;

    if (led_amt <= 0) {
        ESP_LOGE(TAG, "Configured LED amount out of bounds (0 <---> ..)");
        return ESP_FAIL;
    }

    led_amount = led_amt;

    brightness = b;

    led_strip_config_t strip_config = {.strip_gpio_num = gpio_pin,
                                       .max_leds = (uint32_t)led_amt,
                                       .led_model = LED_MODEL_WS2812,
                                       .flags = {
                                           .invert_out = false,
                                       }};

    led_strip_rmt_config_t rmt_config = {.clk_src = RMT_CLK_SRC_DEFAULT,
                                         .resolution_hz = 10 * 1000 * 1000,
                                         .mem_block_symbols =
                                             0, // Let the driver choose
                                         .flags = {
                                             .with_dma = false,
                                         }};

    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config,
                                             &led_strip_handle));

    led_vector.resize(led_amount);
    for (auto &p : led_vector) {
        p.red = p.green = p.blue = 0;
    }

    xTaskCreate(led_manager_trigger_handler, "LED_MANAGER_TASK", 2048, NULL,
                NULL, &xLedTaskHandle);

    return ESP_OK;
}


void led_manager_trigger_handler(void *arg) {
    TickType_t xLastWakeTime;
    BaseType_t xWasDelayed;

    xLastWakeTime = xTaskGetTickCount();

    for (;;) {

        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(1000 / fps));
        xLastWakeTime = xTaskGetTickCount();

        for (int i = 0; i < led_amount; i++) {
            led_vector[i].red = 0;
            led_vector[i].green = 0;
            led_vector[i].blue = 0;
        }

        for (auto it = animation_queue.begin(); it != animation_queue.end();) {
            (*it)->processAnimation(led_vector);

            if ((*it)->getTimeLeft() <= 0) {
                it = animation_queue.erase(it);
            } else {
                it++;
            }
        }

        for (int i = 0; i < led_amount; i++) {
            esp_err_t err = led_strip_set_pixel(
                led_strip_handle, i,
                (uint32_t)(led_vector[i].red) * brightness / 255,
                (uint32_t)(led_vector[i].green) * brightness / 255,
                (uint32_t)(led_vector[i].blue) * brightness / 255);
        }

        led_strip_refresh(led_strip_handle);
    }
}

// Add led effect to animation queue.
void trigger_animation(std::unique_ptr<led_animation> ptr) {
    animation_queue.push_back(std::move(ptr));
}

// Led_animation Constructors.
led_animation::led_animation(uint32_t t_l)
    : time_length(t_l), current_time(0) {}

led_train::led_train(uint32_t t_l, uint8_t r, uint8_t g, uint8_t b)
    : led_animation(t_l), red(r), green(g), blue(b) {}



// Actual implementation of animation.
void led_train::processAnimation(std::vector<led_pixel_t> &strip) {
    for (int i = 0; i < led_amount; i++) {
        if (i == current_time % led_amount) {
            strip[i].red = std::max((uint32_t)red, strip[i].red);
            strip[i].green = std::max((uint32_t)green, strip[i].green);
            strip[i].blue = std::max((uint32_t)blue, strip[i].blue);
        }
    }

    current_time++;
}

led_flash::led_flash(uint32_t t_l, uint8_t r, uint8_t g, uint8_t b)
    : led_animation(t_l), red(r), green(g), blue(b) {}

void led_flash::processAnimation(std::vector<led_pixel_t> &strip) {
    for (int i = 0; i < led_amount; i++) {
        if (getTimeLeft() >= 0) {
            strip[i].red = std::max((uint32_t)red, strip[i].red);
            strip[i].green = std::max((uint32_t)green, strip[i].green);
            strip[i].blue = std::max((uint32_t)blue, strip[i].blue);
        } else {
            strip[i].red = strip[i].green = strip[i].blue = 0;
        }
    }
    current_time++;
}

led_wave_right::led_wave_right(uint8_t spd, uint8_t r, uint8_t g, uint8_t b)
    : led_animation(spd*led_amount), red(r), green(g), blue(b), speed(spd) {}

void led_wave_right::processAnimation(std::vector<led_pixel_t> &strip) {
    for (int i = 0; i < led_amount; i++) {
        if (current_time/speed >= i) {
            strip[i].red = std::max((uint32_t)red, strip[i].red);
            strip[i].green = std::max((uint32_t)green, strip[i].green);
            strip[i].blue = std::max((uint32_t)blue, strip[i].blue);
        } else {
            strip[i].red = strip[i].green = strip[i].blue = 0;
        }
    }
    current_time++;
}

led_wave_left::led_wave_left(uint8_t spd, uint8_t r, uint8_t g, uint8_t b)
    : led_animation(spd*led_amount), red(r), green(g), blue(b), speed(spd) {}

void led_wave_left::processAnimation(std::vector<led_pixel_t> &strip) {
    for (int i = 0; i < led_amount; i++) {
        if ((led_amount-1) - (current_time/speed) <= i) {
            strip[i].red = std::max((uint32_t)red, strip[i].red);
            strip[i].green = std::max((uint32_t)green, strip[i].green);
            strip[i].blue = std::max((uint32_t)blue, strip[i].blue);
        } else {
            strip[i].red = strip[i].green = strip[i].blue = 0;
        }
    }
    current_time++;
}

led_heartbeats::led_heartbeats(uint16_t hb_length, uint8_t r, uint8_t g, uint8_t b)
    : led_animation(hb_length*4 + fps*0.5), heartbeat_length(hb_length), red(r), green(g), blue(b) {}

float calc_intensity(int hb_length,int current_time){
    return std::max((float)(hb_length-std::max(current_time,0))/hb_length,(float)0.0)*(current_time >= 0);
}

float calc_intensity_diff(float current_time){
    return std::max(-0.5 * std::pow(std::sin(0.3*current_time-3),10)+0.8 * std::pow(std::sin(0.3*current_time+4.5),22)+7 * std::pow(std::sin(0.3*current_time),300)+0.5 * std::pow(std::sin(0.3*current_time+0.9),16)+-0.2 * std::pow(std::sin(0.5*current_time),10),0.0)/6.55;
}

void led_heartbeats::processAnimation(std::vector<led_pixel_t> &strip) {

    // float intensity = calc_intensity_diff((float)current_time/2);

    float intensity =   calc_intensity(heartbeat_length,current_time) + 
                        calc_intensity(heartbeat_length,current_time-25) +
                        calc_intensity(heartbeat_length,current_time-75) +
                        calc_intensity(heartbeat_length,current_time-100); 




    for (int i = 0; i < led_amount; i++) {
        float sideintensity = 1-abs(i-((float)led_amount/2))*(1/((float)led_amount)*0.7);

        strip[i].red = std::max((uint32_t)(red*intensity*sideintensity), strip[i].red);
        strip[i].green = std::max((uint32_t)(green*intensity*sideintensity), strip[i].green);
        strip[i].blue = std::max((uint32_t)(blue*intensity*sideintensity), strip[i].blue);

    }
    current_time++;
}
