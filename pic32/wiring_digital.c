#include <Arduino.h>
#include "sdk/gpio.h"
#include "pins_arduino.h"

int pinToGPIO(int pin) {
    if (pin >= NUM_DIGITAL_PINS) {
        return -1;
    }

    if (digitalPinMap[pin] < 0) {
        return -1;
    }
    
    return digitalPinMap[pin];
}

void pinMode(int pin, int mode) {
    int gpio = pinToGPIO(pin);
    if (gpio < 0) return;
    uint8_t m = 0;
    switch (mode) {
        case INPUT:
            m = gpioMODE_INPUT;
            break;
        case OUTPUT:
            m = gpioMODE_OUTPUT;
            break;
        case INPUT_PULLUP:
            m = gpioMODE_INPUT | gpioMODE_PULLUP;
            break;
        case INPUT_PULLDOWN:
            m = gpioMODE_INPUT | gpioMODE_PULLDOWN;
            break;
        case INPUT_PULLUPDOWN:
            m = gpioMODE_INPUT | gpioMODE_PULLDOWN | gpioMODE_PULLUP;
            break;
        case OPEN:
            m = gpioMODE_OUTPUT | gpioMODE_OPENDRAIN;
            break;
    }
    gpio_set_mode(gpio, m);
}

void digitalWrite(int pin, int level) {
    int gpio = pinToGPIO(pin);
    if (gpio < 0) return;
    gpio_write(gpio, level);
}

uint8_t digitalRead(int pin) {
    int gpio = pinToGPIO(pin);
    if (gpio < 0) return 0;
    return gpio_read(gpio);
}

void attachInterrupt(uint8_t pin, void (*callback)(), int mode) {
    int gpio = pinToGPIO(pin);
    if (gpio < 0) return;
    if (mode == RISING) {
        gpio_disconnect_interrupt(gpio, gpioINTERRUPT_FALLING);
        gpio_connect_interrupt(gpio, gpioINTERRUPT_RISING, (gpioISR_t)callback);
    } else if (mode == FALLING) {
        gpio_disconnect_interrupt(gpio, gpioINTERRUPT_RISING);
        gpio_connect_interrupt(gpio, gpioINTERRUPT_FALLING, (gpioISR_t)callback);
    } else if (mode == CHANGE) {
        gpio_connect_interrupt(gpio, gpioINTERRUPT_FALLING, (gpioISR_t)callback);
        gpio_connect_interrupt(gpio, gpioINTERRUPT_RISING, (gpioISR_t)callback);
    }
}

void detachInterrupt(uint8_t pin) {
    gpio_disconnect_interrupt(gpio, gpio_INTERRUPT_FALLING);
    gpio_disconnect_interrupt(gpio, gpio_INTERRUPT_RISING);
}
