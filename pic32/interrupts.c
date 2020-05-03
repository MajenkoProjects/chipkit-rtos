#include <Arduino.h>
#include "sdk/cpu.h"

// Yes, I know this isn't strictly a "restore", but it's only ever used to turn back on.
void  restoreInterrupts(uint32_t st) {
    cpu_enable_interrupts();
}

uint32_t disableInterrupts(void) {
    cpu_disable_interrupts();
    return 1;
}

uint32_t enableInterrupts(void) {
    cpu_enable_interrupts();
    return 0;
}

