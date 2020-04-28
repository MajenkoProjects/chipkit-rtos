#if defined(__PIC32MX__)
#   include "targets/MX/port_asm.S"
#elif defined(__PIC32MZ__)
#   include "targets/MZ/port_asm.S"
#else
#   error "No target for your MCU"
#endif
