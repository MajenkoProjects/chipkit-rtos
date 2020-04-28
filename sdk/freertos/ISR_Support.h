#if defined(__PIC32MX__)
#   include "targets/MX/ISR_Support.h"
#elif defined(__PIC32MZ__)
#   include "targets/MZ/ISR_Support.h"
#else
#   error "No target for your MCU"
#endif
