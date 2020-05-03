#ifndef _SDK_UART_H
#define _SDK_UART_H

#include <stdint.h>
#include "stream_buffer.h"

#define uart8N1 0b000
#define uart8N2 0b001
#define uart8E1 0b010
#define uart8E2 0b011
#define uart8O1 0b100
#define uart8O2 0b101
#define uart9N1 0b110
#define uart9N2 0b111

#ifdef __cplusplus
extern "C" {
#endif

// This is the type used to transfer data around for the UART.
// 16 bits are needed so that it's possible to use 9-bit mode.
typedef uint16_t uart_queue_t;


extern int uart_rx_available(uint8_t uart);
extern int uart_tx_available(uint8_t uart);
extern int uart_peek(uint8_t uart);
extern int uart_read(uint8_t uart);
extern int uart_write_bytes(uint8_t uart, const uint8_t *bytes, size_t len);
extern int uart_write(uint8_t uart, uart_queue_t byte);
extern int uart_set_tx_pin(uint8_t uart, uint8_t pin);
extern int uart_set_rx_pin(uint8_t uart, uint8_t pin);
extern int uart_set_baud(uint8_t uart, uint32_t baud);
extern int uart_set_format(uint8_t uart, uint8_t format);
extern int uart_open(uint8_t uart);
extern int uart_close(uint8_t uart);
extern void uart_flush(uint8_t uart);
extern void uart_purge(uint8_t uart);
extern int uart_write_bytes_emergency(uint8_t uart, const uint8_t *bytes, size_t len);
extern int uart_is_open(uint8_t uart);

#ifdef __cplusplus
}
#endif

#endif
