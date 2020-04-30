#include <Arduino.h>

#include "FreeRTOS.h"
#include "task.h"

#include "sdk/cpu.h"

void delay(uint32_t ms) {
    vTaskDelay(ms * portTICK_PERIOD_MS);
}

uint32_t millis() {
    return xTaskGetTickCount() * portTICK_PERIOD_MS;
}

uint32_t micros() {
    uint32_t ctc;
    cpu_ct_read_count(ctc);
    return ctc / (cpu_get_system_clock() / 2 / 1000000);
}
