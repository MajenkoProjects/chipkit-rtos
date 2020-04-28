#include <Arduino.h>
#include "FreeRTOS.h"
#include "task.h"

// Yes, I know this isn't strictly a "restore", but it's only ever used to turn back on.
void  restoreInterrupts(uint32_t st) {
    taskENABLE_INTERRUPTS();
}

uint32_t disableInterrupts(void) {
    taskDISABLE_INTERRUPTS();
}

uint32_t enableInterrupts(void) {
    taskENABLE_INTERRUPTS();
}

