#ifndef _SDK_CHIPSPEC_H
#define _SDK_CHIPSPEC_H

#if defined(__32MZ0512EFE064__)
    #define __CHIP_NAME                     "32MZ0512EFE064"
    #define __CHIP_PINS                     64
    #define __CHIP_HAS_PPS                  1
    #define __CHIP_HAS_USB                  1
    #define __CHIP_HAS_UART                 6
    #define __CHIP_HAS_DMA                  8
    #define __CHIP_FAMILY                   MZ
    #define __CHIP_SUBFAMILY                EF
    #define __CHIP_HAS_ETHERNET             1
    #define __CHIP_FLASH_ROW                512
    #define __CHIP_FLASH_PAGE               4096
    #define __CHIP_FLASH_SIZE               (512 * 1024L) 
    #define __CHIP_RAM_SIZE                 (256 * 1024L)
#endif


#endif
