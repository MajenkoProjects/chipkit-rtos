#include <Arduino.h>

#include "FreeRTOS.h"
#include "task.h"

void delay(uint32_t ms) {
    vTaskDelay(ms * portTICK_PERIOD_MS);
}

uint32_t millis() {
    return xTaskGetTickCount() * portTICK_PERIOD_MS;
}
