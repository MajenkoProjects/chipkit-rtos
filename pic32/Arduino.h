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

#define LOW     0x0
#define HIGH    0x1
#define CHANGE  0x2
#define FALLING 0x3
#define RISING  0x4

#define INPUT               0x0
#define OUTPUT              0x1
#define OPEN                0x2
#define INPUT_PULLUP        0x3
#define INPUT_PULLDOWN      0x4
#define INPUT_PULLUPDOWN    0x5

#ifdef __cplusplus
extern "C" {
#endif

extern void delay(uint32_t ms);
extern uint32_t millis();

extern void pinMode(int, int);
extern void digitalWrite(int, int);
extern uint8_t digitalRead(int pin);

#define digitalPinToInterrupt(X) (X)

extern void attachInterrupt(uint8_t interruptNum, void (*callback)(), int mode);
extern void detachInterrupt(uint8_t interruptNum);

extern void __attribute__((nomips16))  restoreInterrupts(uint32_t st);
extern uint32_t __attribute__((nomips16)) disableInterrupts(void);
extern uint32_t __attribute__((nomips16))  enableInterrupts(void);

#ifdef __cplusplus
}
#endif

#endif
