#ifndef _SDK_FLASH_H
#define _SDK_FLASH_H

#ifdef __cplusplus
extern "C" {
#endif

extern void flash_unlock();
extern void flash_lock();
extern int flash_is_ok();
extern int __attribute__((nomips16)) flash_operation(uint32_t op);
extern int flash_erase_page(void* adr);
extern int flash_write_word(void* adr, uint32_t val);
extern int flash_write_row(void* adr, void *val);
extern int flash_clear_error();
extern int flash_erase_progmem();

#ifdef __cplusplus
}
#endif


#define flashOP_NOP                 0x4000
#define flashOP_WRITE_WORD          0x4001
#define flashOP_WRITE_ROW           0x4003
#define flashOP_ERASE_PAGE          0x4004
#define flashOP_ERASE_PROGMEM       0x4007

#endif
