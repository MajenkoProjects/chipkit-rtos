#if defined(__PIC32MX__)
#   include "targets/MX/portmacro.h"
#elif defined(__PIC32MZ__)
#   include "targets/MZ/portmacro.h"
#else
#   error "No target for your MCU"
#endif
