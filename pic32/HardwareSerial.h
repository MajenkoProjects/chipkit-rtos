#ifndef _HARDWARE_SERIAL_H
#define _HARDWARE_SERIAL_H

#include <Arduino.h>

#include "FreeRTOS.h"
#include "sdk/uart.h"
#include "sdk/chipspec.h"

#include "pins_arduino.h"

class HardwareSerial : public Stream {
    private:
        uint8_t _uart;
        uint8_t _txPin;
        uint8_t _rxPin;

    public:
#if defined(__CHIP_HAS_PPS)
        HardwareSerial(uint8_t uart, uint8_t tx, uint8_t rx) :
            _uart(uart), _txPin(tx), _rxPin(rx) {}
#else
        HardwareSerial(uint8_t uart) :
            _uart(uart) {}
#endif
        void            begin(unsigned long baudRate);
        void            end();
        virtual int     available();
        virtual int     availableForWrite();
        virtual int     peek();
        virtual int     read();
        virtual void    flush();
        virtual void    purge();
        virtual size_t  write(uint8_t);
        virtual size_t write(const uint8_t *buffer, size_t size);
        using   Print::write; // pull in write(str) and write(buf, size) from Print
        operator        int();
};

#ifdef SERIAL0
#if (__CHIP_HAS_PPS)
HardwareSerial Serial(SERIAL0, SERIAL0_TX, SERIAL0_RX);
#else
HardwareSerial Serial(SERIAL0);
#endif
#endif

#ifdef SERIAL1
#if (__CHIP_HAS_PPS)
HardwareSerial Serial(SERIAL1, SERIAL1_TX, SERIAL1_RX);
#else
HardwareSerial Serial(SERIAL1);
#endif
#endif

#ifdef SERIAL2
#if (__CHIP_HAS_PPS)
HardwareSerial Serial(SERIAL2, SERIAL2_TX, SERIAL2_RX);
#else
HardwareSerial Serial(SERIAL2);
#endif
#endif

#ifdef SERIAL3
#if (__CHIP_HAS_PPS)
HardwareSerial Serial(SERIAL3, SERIAL3_TX, SERIAL3_RX);
#else
HardwareSerial Serial(SERIAL3);
#endif
#endif

#ifdef SERIAL4
#if (__CHIP_HAS_PPS)
HardwareSerial Serial(SERIAL4, SERIAL4_TX, SERIAL4_RX);
#else
HardwareSerial Serial(SERIAL4);
#endif
#endif

#ifdef SERIAL5
#if (__CHIP_HAS_PPS)
HardwareSerial Serial(SERIAL5, SERIAL5_TX, SERIAL5_RX);
#else
HardwareSerial Serial(SERIAL5);
#endif
#endif

#endif
