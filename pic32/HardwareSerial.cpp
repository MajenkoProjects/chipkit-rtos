#include "HardwareSerial.h"

void HardwareSerial::begin(unsigned long baudRate) {
    uart_set_baud(_uart, baudRate);
#if (__CHIP_HAS_PPS)
    uart_set_tx_pin(_uart, digitalPinMap[_txPin]);
    uart_set_rx_pin(_uart, digitalPinMap[_rxPin]);
#endif
    uart_open(_uart);
}

void HardwareSerial::end() {
    uart_close(_uart);
}

int HardwareSerial::available() {
    return uart_rx_available(_uart);
}

int HardwareSerial::availableForWrite() {
    return uart_tx_available(_uart);
}

int HardwareSerial::peek() {
    return uart_peek(_uart);
}

int HardwareSerial::read() {
    return uart_read(_uart);
}

void HardwareSerial::flush() {
    uart_flush(_uart);
}

void HardwareSerial::purge() {
    uart_purge(_uart);
}

size_t HardwareSerial::write(uint8_t val) {
    return uart_write(_uart, val);
}

size_t HardwareSerial::write(const uint8_t *buffer, size_t size) {
    return uart_write_bytes(_uart, buffer, size);
}

