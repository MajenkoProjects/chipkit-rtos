#if defined(__PIC32MX__)
#   include "targets/MX/FreeRTOSConfig.h"
#elif defined(__PIC32MZ__)
#   include "targets/MZ/FreeRTOSConfig.h"
#else
#   error "No target for your MCU"
#endif
