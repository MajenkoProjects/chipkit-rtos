#include <p32xxxx.h>
#include <stdint.h>

#include "sdk/cpu.h"

uint32_t cpu_get_peripheral_clock() {
    uint32_t    clock = F_CPU;
#if defined(__PIC32MZ__)
    clock = (F_CPU / (PB2DIVbits.PBDIV + 1));
#else
    clock >>= OSCCONbits.PBDIV;
#endif
    return clock;
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
    uint32_t        st;

    iec = ((p32_regset *)&IEC0) + (irq / 32);
    st = iec->reg;
    return st;
}

void cpu_set_interrupt_priority(uint8_t vec, uint8_t ipl, uint8_t spl) {
    p32_regset *    ipc;
    int             bn;

    ipc = ((p32_regset *)&IPC0) + (vec / 4);
    bn = 8 * (vec % 4);
    ipc->clr = (0x1F << bn);
    ipc->set = ((ipl << 2) + spl) << bn;
}

