#ifndef _ARDUINO_H
#define _ARDUINO_H

#include <p32xxxx.h>
#include <stdint.h>
#include <stdbool.h>

#include "pins_arduino.h"

#ifdef __cplusplus
#include "WString.h"
#include "Stream.h"
#include "Print.h"
#include "HardwareSerial.h"
#endif

#define HIGH 0x1
#define LOW  0x0

#define INPUT 0x0
#define OUTPUT 0x1
#define OPEN 0x02
#define INPUT_PULLUP 0x03
#define INPUT_PULLDOWN 0x04
#define INPUT_PULLUPDOWN 0x05

#ifdef __cplusplus
extern "C" {
#endif

extern void delay(uint32_t ms);
extern uint32_t millis();

void pinMode(int, int);
void digitalWrite(int, int);
uint8_t digitalRead(int pin);

extern void __attribute__((nomips16))  restoreInterrupts(uint32_t st);
extern uint32_t __attribute__((nomips16)) disableInterrupts(void);
extern uint32_t __attribute__((nomips16))  enableInterrupts(void);

#ifdef __cplusplus
}
#endif

#endif
