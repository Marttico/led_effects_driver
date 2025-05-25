#include "esp_check.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led_strip.h"
#include <algorithm>
#include <memory>
#include <string>
#include <vector>
#include <bits/stdc++.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <png.h>


class png_image_led{
    private:
        const uint8_t *png_data_start, *png_data_end;
        size_t png_size;
        png_bytep *rows;
        int width, height, channels;
        bool loaded;
    
    public:
    png_image_led(const uint8_t *png_data_start, const uint8_t *png_data_end);
    esp_err_t readPixel(uint32_t x, uint32_t y, uint8_t &r, uint8_t &g, uint8_t &b);
    int getWidth(),getHeight();
    ~png_image_led();
};


void testPNG();