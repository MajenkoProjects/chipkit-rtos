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

    if (mode != CHANGE) {
        switch (pin) {
#if defined(PIN_INT0)
            case PIN_INT0:
                gpio_connect_external_interrupt(gpio, 0, mode, callback);
                return;
#endif
#if defined(PIN_INT1)
            case PIN_INT1:
                gpio_connect_external_interrupt(gpio, 1, mode, callback);
                return;
#endif
#if defined(PIN_INT2)
            case PIN_INT2:
                gpio_connect_external_interrupt(gpio, 2, mode, callback);
                return;
#endif
#if defined(PIN_INT3)
            case PIN_INT3:
                gpio_connect_external_interrupt(gpio, 3, mode, callback);
                return;
#endif
#if defined(PIN_INT4)
            case PIN_INT4:
                gpio_connect_external_interrupt(gpio, 4, mode, callback);
                return;
#endif
        }
    }

    if (gpio < 0) return;
    if (mode == RISING) {
        gpio_disconnect_change_interrupt(gpio, gpioINTERRUPT_FALLING);
        gpio_connect_change_interrupt(gpio, gpioINTERRUPT_RISING, (gpioISR_t)callback);
    } else if (mode == FALLING) {
        gpio_disconnect_change_interrupt(gpio, gpioINTERRUPT_RISING);
        gpio_connect_change_interrupt(gpio, gpioINTERRUPT_FALLING, (gpioISR_t)callback);
    } else if (mode == CHANGE) {
        gpio_connect_change_interrupt(gpio, gpioINTERRUPT_FALLING, (gpioISR_t)callback);
        gpio_connect_change_interrupt(gpio, gpioINTERRUPT_RISING, (gpioISR_t)callback);
    }
}

void detachInterrupt(uint8_t pin) {
    switch (pin) {
#if defined(PIN_INT0)
        case PIN_INT0: gpio_disconnect_external_interrupt(0); break;
#endif
#if defined(PIN_INT1)
        case PIN_INT1: gpio_disconnect_external_interrupt(1); break;
#endif
#if defined(PIN_INT2)
        case PIN_INT2: gpio_disconnect_external_interrupt(2); break;
#endif
#if defined(PIN_INT3)
        case PIN_INT3: gpio_disconnect_external_interrupt(3); break;
#endif
#if defined(PIN_INT4)
        case PIN_INT4: gpio_disconnect_external_interrupt(4); break;
#endif
    }

    int gpio = pinToGPIO(pin);
    if (gpio < 0) return;
    gpio_disconnect_change_interrupt(gpio, gpioINTERRUPT_FALLING);
    gpio_disconnect_change_interrupt(gpio, gpioINTERRUPT_RISING);
}
