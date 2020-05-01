#include <p32xxxx.h>

#include "FreeRTOS.h"
#include "sdk/flash.h"
#include "sdk/chipspec.h"

void flash_unlock() {
    NVMKEY = 0xAA996655;
    NVMKEY = 0x556699AA;
}

void flash_lock() {
    NVMKEY = 0x33333333;
}

int flash_is_ok() {
    return (NVMCON & (_NVMCON_WRERR_MASK | _NVMCON_LVDERR_MASK)) ? 1 : 0;
}

int __attribute__((nomips16)) flash_operation(uint32_t op) {
    cpu_disable_interrupts();

    NVMCON = _NVMCON_WREN_MASK | op;

    flash_unlock();

    NVMCONSET = _NVMCON_WR_MASK;
    while (NVMCON & _NVMCON_WR_MASK) {
        continue;
    }
    NVMCONCLR = _NVMCON_WR_MASK;
    flash_lock();

    cpu_enable_interrupts();
    return flash_is_ok();
}

int flash_erase_page(void* adr) {
    NVMADDR = KVA_TO_PA((unsigned int) adr);
    return flash_operation(flashOP_ERASE_PAGE);
}

int flash_write_word(void* adr, uint32_t val) {
    NVMADDR = KVA_TO_PA((unsigned int) adr);
#if defined(__PIC32MZ__)
    NVMDATA0 = val;
#elif defined(__PIC32MX__)
    NVMDATA = val;
#else
#error "No support for your CPU"
#endif
    return flash_operation(flashOP_WRITE_WORD);
}

int flash_write_row(void* adr, void *val) {
    int blank = 1;
    for (int i = 0; i < __CHIP_FLASH_ROW; i++) {
        if (((uint8_t *)val)[i] != 0xFF) {
            blank = 0;
        }
    }
    if (blank) {
        return 1;
    }
    NVMADDR = KVA_TO_PA((unsigned int) adr);
    NVMSRCADDR = KVA_TO_PA((uint32_t)val);
    return flash_operation(flashOP_WRITE_ROW);
}

int flash_clear_error() {
    return flash_operation(flashOP_NOP);
}

int flash_erase_progmem() {
    return flash_operation(flashOP_ERASE_PROGMEM);
}
