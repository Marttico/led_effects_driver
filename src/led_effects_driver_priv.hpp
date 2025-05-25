
#include "esp_check.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
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

void led_manager_trigger_handler(void *arg);

