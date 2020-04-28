#include "FreeRTOS.h"
#include "task.h"

void vAssertCalled( const char * pcFile, unsigned long ulLine ) {
}

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName ) {
    TRISECLR = 1 << 6;
    LATESET = 1 << 6;
    taskDISABLE_INTERRUPTS();
    for( ;; );
}

void vApplicationMallocFailedHook( TaskHandle_t pxTask, char *pcTaskName ) {
    TRISECLR = 1 << 6;
    LATESET = 1 << 6;
    taskDISABLE_INTERRUPTS();
    for( ;; );
}


