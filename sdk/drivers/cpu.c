#include <p32xxxx.h>
#include <stdint.h>

#include "sdk/cpu.h"

uint32_t cpu_get_peripheral_clock() {
#if defined(__PIC32MZ__)
    return (cpu_get_system_clock() / (PB2DIVbits.PBDIV + 1));
#elif defined(__PIC32MX__)
    uint32_t    clock = cpu_get_system_clock();
    clock >>= OSCCONbits.PBDIV;
    return clock;
#else
    #error "No support for your CPU, sorry"
#endif
}

// TODO: Calculate the clock from the incoming clock source and PLL settings
uint32_t cpu_get_system_clock() {
    return F_CPU;
}

int cpu_get_interrupt_flag(uint8_t irq) {
    p32_regset *    ifs;

    ifs = ((p32_regset *)&IFS0) + (irq / 32);
    return (ifs->reg & (1 << (irq % 32))) != 0;
}

void cpu_set_interrupt_flag(uint8_t irq) {
    p32_regset *    ifs;

    ifs = ((p32_regset *)&IFS0) + (irq / 32);
    ifs->set = 1 << (irq % 32);
}

void cpu_clear_interrupt_flag(uint8_t irq) {
    p32_regset *    ifs;

    ifs = ((p32_regset *)&IFS0) + (irq / 32);
    ifs->clr = 1 << (irq % 32);
}

void cpu_set_interrupt_enable(uint8_t irq) {
    p32_regset *    iec;

    iec = ((p32_regset *)&IEC0) + (irq / 32);
    iec->set = 1 << (irq % 32);
}

void cpu_clear_interrupt_enable(uint8_t irq) {
    p32_regset *    iec;

    iec = ((p32_regset *)&IEC0) + (irq / 32);
    iec->clr = 1 << (irq % 32);
}

int cpu_get_interrupt_enable(uint8_t irq) {
    p32_regset *    iec;

    iec = ((p32_regset *)&IEC0) + (irq / 32);
    return (iec->reg & (1 << (irq % 32))) ? 1 : 0;
}

void cpu_set_interrupt_priority(uint8_t vec, uint8_t ipl, uint8_t spl) {
    p32_regset *    ipc;
    int             bn;

    ipc = ((p32_regset *)&IPC0) + (vec / 4);
    bn = 8 * (vec % 4);
    ipc->clr = (0x1F << bn);
    ipc->set = ((ipl << 2) + spl) << bn;
}

void cpu_unlock() {
    SYSKEY = 0;
    SYSKEY = 0xAA996655;
    SYSKEY = 0x556699AA;
}

void cpu_lock() {
    SYSKEY = 0x33333333;
}

void cpu_reset() {
    cpu_disable_interrupts();
    cpu_unlock();
    RSWRSTSET=_RSWRST_SWRST_MASK;
}

/**
 * Initialize the Core Timer to zero and set an initial compare
 * @param: initcompare Initial value to use in the Compare register
 */

void cpu_ct_init(uint32_t initcompare) {
    asm volatile("mtc0 $0, $9");
    asm volatile("mtc0 %0, $11" : "+r"(initcompare));
}
