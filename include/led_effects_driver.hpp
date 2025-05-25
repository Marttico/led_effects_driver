#include "esp_err.h"
#include "esp_log.h"
#include "led_strip.h"
#include <memory>
#include <sys/time.h>
#include <vector>
#include "png_loader.hpp"



struct led_pixel_t {
    uint32_t red;
    uint32_t green;
    uint32_t blue;
};

class led_animation {
  protected:
    uint32_t time_length;
    uint32_t current_time;

  public:
    led_animation(uint32_t t_l);
    int64_t getTimeLeft() {
        return led_animation::time_length - led_animation::current_time;
    };
    void set_time_length(uint32_t t_l){time_length = t_l;};
    virtual void processAnimation(std::vector<led_pixel_t> &strip) = 0;
};

class led_train : public led_animation {
  private:
    uint8_t red, green, blue;

  public:
    led_train(uint32_t t_l, uint8_t r, uint8_t g, uint8_t b);
    void processAnimation(std::vector<led_pixel_t> &strip) override;
};

class led_flash : public led_animation {
  private:
    uint8_t red, green, blue;

  public:
    led_flash(uint32_t t_l, uint8_t r, uint8_t g, uint8_t b);
    void processAnimation(std::vector<led_pixel_t> &strip) override;
};

class led_wave_right : public led_animation {
  private:
    uint8_t red, green, blue, speed;

  public:
  led_wave_right(uint8_t speed, uint8_t r, uint8_t g, uint8_t b);
    void processAnimation(std::vector<led_pixel_t> &strip) override;
};

class led_wave_left : public led_animation {
  private:
    uint8_t red, green, blue, speed;

  public:
  led_wave_left(uint8_t speed, uint8_t r, uint8_t g, uint8_t b);
    void processAnimation(std::vector<led_pixel_t> &strip) override;
};

class led_heartbeats : public led_animation {
  private:
  uint16_t heartbeat_length;
    uint8_t red, green, blue;

  public:
  led_heartbeats(uint16_t hb_length, uint8_t r, uint8_t g, uint8_t b);
    void processAnimation(std::vector<led_pixel_t> &strip) override;
};

class led_png_animation : public led_animation {
  private:
    png_image_led p;
  public:
    led_png_animation(const uint8_t *png_data_start, const uint8_t *png_data_end);
    void processAnimation(std::vector<led_pixel_t> &strip) override;
};

void led_set_global_vector(int i, uint8_t r, uint8_t g, uint8_t b);

esp_err_t init_led_effects_driver(int gpio_pin, int led_amt);

esp_err_t init_led_effects_driver(int gpio_pin, int led_amt, uint8_t b);

esp_err_t led_clear_everything();

/*
Example:
- led_trigger_animation(std::make_unique<led_heartbeats>(25, 251, 111, 146));
- led_trigger_animation(std::make_unique<led_png_animation>(love_start,love_end));


*/
void led_trigger_animation(std::unique_ptr<led_animation> entry);

/*Idea of this led driver is to calculate all the brightnesses and colors of
  LEDs before actually applying them. This way multiple animations can be shown
  at once. Using a queue queue and preset animations this can be done. If queue
  is empty, suspend task.

*/