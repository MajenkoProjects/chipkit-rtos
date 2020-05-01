/**
 * @file cpu.c
 *
 * Controls the internals of the CPU, interrupts, clocks, etc.
 */
#include <p32xxxx.h>
#include <stdint.h>

#include "sdk/cpu.h"

/**
 * Calculates and returns the current peripheral bus clock frequency
 * @returns The frequency in Hz
 */
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

/**
 * Calculates and returns the current CPU clock frequency
 * @returns The frequency in Hz
 */
// TODO: Calculate the clock from the incoming clock source and PLL settings
uint32_t cpu_get_system_clock() {
    return F_CPU;
}

/**
 * Tests if an interrupt flag is set or not
 * @param irq The vector (MZ) or IRQ (MX) of the interrupt you are querying
 * @returns 1 if the flag is set, otherwise 0.
 */
int cpu_get_interrupt_flag(uint8_t irq) {
    p32_regset *    ifs;

    ifs = ((p32_regset *)&IFS0) + (irq / 32);
    return (ifs->reg & (1 << (irq % 32))) != 0;
}

/**
 * Set an interrupt flag. This has the effect of immediately causing the interrupt
 * service handler to be called.
 * @param irq The vector (MZ) or IRQ (MX) of the interrupt to set the flag for
 */
void cpu_set_interrupt_flag(uint8_t irq) {
    p32_regset *    ifs;

    ifs = ((p32_regset *)&IFS0) + (irq / 32);
    ifs->set = 1 << (irq % 32);
}

/**
 * Clear an interrupt flag.  This should normally be performed in the interrupt
 * service handler for an interrupt.
 * @param irq The vector (MZ) or IRQ (MX) of the interrupt to clear the flag for
 */
void cpu_clear_interrupt_flag(uint8_t irq) {
    p32_regset *    ifs;

    ifs = ((p32_regset *)&IFS0) + (irq / 32);
    ifs->clr = 1 << (irq % 32);
}

/**
 * Enable an interrupt
 * @param irq The vector (MZ) or IRQ (MX) of the interrupt to enable
 */
void cpu_set_interrupt_enable(uint8_t irq) {
    p32_regset *    iec;

    iec = ((p32_regset *)&IEC0) + (irq / 32);
    iec->set = 1 << (irq % 32);
}

/**
 * Disable an interrupt
 * @param irq The vector (MZ) or IRQ (MX) of the interrupt to disable
 */
void cpu_clear_interrupt_enable(uint8_t irq) {
    p32_regset *    iec;

    iec = ((p32_regset *)&IEC0) + (irq / 32);
    iec->clr = 1 << (irq % 32);
}

/**
 * Query if an interrupt is enabled or not
 * @param irq The vector (MZ) or IRQ (MX) of the interrupt to query
 * @returns 1 if the interrupt is enabled, 0 otherwise
 */
int cpu_get_interrupt_enable(uint8_t irq) {
    p32_regset *    iec;

    iec = ((p32_regset *)&IEC0) + (irq / 32);
    return (iec->reg & (1 << (irq % 32))) ? 1 : 0;
}

/**
 * Set the priority and sub-priority for an interrupt
 * @param irq The vector of the interrupt to set the priority of
 * @param ipl The priority (0-7) for the interrupt
 * @param spl The sub-priority (0-3) for the interrupt
 */
void cpu_set_interrupt_priority(uint8_t vec, uint8_t ipl, uint8_t spl) {
    p32_regset *    ipc;
    int             bn;

    ipc = ((p32_regset *)&IPC0) + (vec / 4);
    bn = 8 * (vec % 4);
    ipc->clr = (0x1F << bn);
    ipc->set = ((ipl << 2) + spl) << bn;
}

/**
 * Unlock protected functions of the CPU
 */
void cpu_unlock() {
    SYSKEY = 0;
    SYSKEY = 0xAA996655;
    SYSKEY = 0x556699AA;
}

/**
 * Lock protected functions of the CPU
 */
void cpu_lock() {
    SYSKEY = 0x33333333;
}

/**
 * Perform a software reset of the CPU
 */
void cpu_reset() {
    cpu_disable_interrupts();
    cpu_unlock();
    RSWRSTSET=_RSWRST_SWRST_MASK;
}

/**
 * Initialize the Core Timer to zero and set an initial compare
 * @param initcompare Initial value to use in the Compare register
 */
void cpu_ct_init(uint32_t initcompare) {
    asm volatile("mtc0 $0, $9");
    asm volatile("mtc0 %0, $11" : "+r"(initcompare));
}
