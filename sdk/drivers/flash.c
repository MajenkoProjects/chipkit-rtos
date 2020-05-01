/**
 * @file flash.c
 *
 * Provides facilities for reading and writing the internal flash of the PIC32
 */
#include <p32xxxx.h>

#include "FreeRTOS.h"
#include "sdk/flash.h"
#include "sdk/chipspec.h"

/**
 * Unlock the flash controller. This must be done before any flash operations are attempted.
 */
void flash_unlock() {
    NVMKEY = 0xAA996655;
    NVMKEY = 0x556699AA;
}

/**
 * Lock the flash controller. This should be done after use.
 */
void flash_lock() {
    NVMKEY = 0x33333333;
}

/**
 * Test the status of the previous flash operation
 * @returns 1 if the operation succeeded, 0 otherwise
 */
int flash_is_ok() {
    return (NVMCON & (_NVMCON_WRERR_MASK | _NVMCON_LVDERR_MASK)) ? 1 : 0;
}

/**
 * Perform a flash operation. The relevent flash registers must be set up first.
 * @param op The operation to perform. One of:
 * * flashOP_NOP
 * * flashOP_WRITE_WORD
 * * flashOP_WRITE_ROW
 * * flashOP_ERASE_PAGE
 * * flashOP_ERASE_PROGMEM
 * @returns 1 if the operation succeeded, 0 otherwise
 */
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

/**
 * Erase a page of flash memory
 * @param addr The address of the page to erase
 * @returns 1 if the operation succeeded, 0 otherwise
 */
int flash_erase_page(void* adr) {
    NVMADDR = KVA_TO_PA((unsigned int) adr);
    return flash_operation(flashOP_ERASE_PAGE);
}

/**
 * Write a single word of data into flash memory. The memory
 * must already have been erased
 * @param adr The destination address to write to
 * @param val The 32-bit word to write
 * @returns 1 if the operation succeeded, 0 otherwise
 */
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

/**
 * Write a row of data into flash memory. The memory
 * must already have been erased
 * @param adr The start address of the row to write to
 * @param val Pointer to the data to write into the row
 * @returns 1 if the operation succeeded, 0 otherwise
 */
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

/**
 * Clear an error from the previous flash operation
 * @returns 1 if the operation succeeded, 0 otherwise
 */
int flash_clear_error() {
    return flash_operation(flashOP_NOP);
}

/**
 * Erase the entirety of the program flash memory. This should only be
 * executed from the boot segment
 * @returns 1 if the operation succeeded, 0 otherwise
 */
int flash_erase_progmem() {
    return flash_operation(flashOP_ERASE_PROGMEM);
}
