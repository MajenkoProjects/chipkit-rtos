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
}

int HardwareSerial::availableForWrite() {
}

int HardwareSerial::peek() {
}

int HardwareSerial::read() {
    return uart_read(_uart);
}

void HardwareSerial::flush() {
}

void HardwareSerial::purge() {
}

size_t HardwareSerial::write(uint8_t val) {
    return uart_write(_uart, val);
}
