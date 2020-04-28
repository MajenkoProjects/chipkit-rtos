#include <Arduino.h>

#include "FreeRTOS.h"
#include "task.h"

extern void setup();
extern void loop();

#define hwUNLOCK_KEY_0                  ( 0xAA996655UL )
#define hwUNLOCK_KEY_1                  ( 0x556699AAUL )

static void arduinoTask(void *params) {
    setup();

    while (1) {
        loop();
    }
}

void __attribute__ ((nomips16)) _configSystem(uint32_t clk)
{
    uint32_t    stInt;
#if defined(_CHECON_PREFEN_POSITION)
    uint32_t    stCache;
    uint32_t    wait;
    register unsigned long tmp;
#endif

    // Added to prevent compiler warning about unused parameter
    (void)clk;

    stInt = disableInterrupts();

    /* Disable wait states in data ram.
    */
#if defined(__PIC32MZ__)
#else
    BMXCONCLR = (1 << _BMXCON_BMXWSDRM_POSITION);
#endif

#if defined(_CHECON_PREFEN_POSITION)

    stCache = CHECON;

    /* Configure predictive prefetch caching for both cached and
    ** non-cached memory regions.
    */
    stCache |= (3 << _CHECON_PREFEN_POSITION);

    /* Turn on caching for KSEG0
    */
    asm("mfc0 %0,$16,0" :  "=r"(tmp));
    tmp = (tmp & ~7) | 3;
    asm("mtc0 %0,$16,0" :: "r" (tmp));

    /* Configure the number of wait states in the program flash
    */
    wait = 0;

    while(clk > FLASH_SPEED_HZ)
    {
        wait += 1;
        clk -= FLASH_SPEED_HZ;
    }

    stCache &= ~_CHECON_PFMWS_MASK;
    stCache |= (wait << _CHECON_PFMWS_POSITION);

    CHECON = stCache;

#elif defined(_PRECON_PREFEN_POSITION)
  // 
  // Set wait states and enable prefetch buffer 
  // 
#if (F_CPU > 200000000UL)
  PRECON = 0u
         | (4u << _PRECON_PFMWS_POSITION)  // 4 wait states 
         | (3u << _PRECON_PREFEN_POSITION); // Enable prefetch for instructions + data 
#else
  PRECON = 0u
         | (2u << _PRECON_PFMWS_POSITION)  // 2 wait states 
         | (3u << _PRECON_PREFEN_POSITION); // Enable prefetch for instructions + data 
#endif

#endif


    restoreInterrupts(stInt);
}

void __attribute__ ((nomips16)) _enableMultiVectorInterrupts()
{

#if 0
    unsigned int val;

    /* Set the CP0 bit so that interrupt exceptions use the
    ** special interrupt vector and not the general exception vector.
    */
    asm volatile("mfc0   %0,$13" : "=r"(val));
    val |= 0x00800000;
    asm volatile("mtc0   %0,$13" : "+r"(val));

    /* Turn on multi-vectored interrupts.
    */
    INTCONSET = _INTCON_MVEC_MASK;

// set up some default shadow registers for each interrupt priority level
// the shadow register set used is the same as the priority level
#if defined(__PIC32MZ__)
        PRISS = 0x76543210;
#endif

    /* Enable interrupts.
    */
    enableInterrupts();
#else 
    _CP0_BIS_CAUSE( 0x00800000U );
    INTCONSET = _INTCON_MVEC_MASK;
    __builtin_enable_interrupts();
#endif
}


void init() {
    _configSystem(F_CPU);
    _enableMultiVectorInterrupts();
}

int main() {

    init();

    xTaskCreate(arduinoTask, "Arduino", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, NULL);
    
    vTaskStartScheduler();

    for (;;);
    return 0;
}
