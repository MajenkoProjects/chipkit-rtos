#ifndef _SDK_CPU_H
#define _SDK_CPU_H

typedef struct {
    volatile uint32_t   reg;
    volatile uint32_t   clr;
    volatile uint32_t   set;
    volatile uint32_t   inv;
} p32_regset;

#ifdef __cplusplus
extern "C" {
#endif

extern uint32_t cpu_get_peripheral_clock();
extern int cpu_get_interrupt_flag(uint8_t);
extern void cpu_set_interrupt_flag(uint8_t);
extern void cpu_clear_interrupt_flag(uint8_t);
extern void cpu_set_interrupt_enable(uint8_t);
extern void cpu_clear_interrupt_enable(uint8_t);
extern int cpu_get_interrupt_enable(uint8_t);
extern void cpu_set_interrupt_priority(uint8_t, uint8_t, uint8_t);

#ifdef __cplusplus
}
#endif

#endif