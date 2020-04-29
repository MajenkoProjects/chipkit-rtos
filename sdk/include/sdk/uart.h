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

int uart_rx_available(uint8_t uart);
int uart_tx_available(uint8_t uart);
int uart_read(uint8_t uart);
int uart_write(uint8_t uart, uint8_t byte);
int uart_set_tx_pin(uint8_t uart, uint8_t pin);
int uart_set_rx_pin(uint8_t uart, uint8_t pin);
int uart_set_baud(uint8_t uart, uint32_t baud);
int uart_set_format(uint8_t uart, uint8_t format);
int uart_open(uint8_t uart);
int uart_close(uint8_t uart);

#ifdef __cplusplus
}
#endif

#endif
