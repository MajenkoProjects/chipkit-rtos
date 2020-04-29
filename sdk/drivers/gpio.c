#include <Arduino.h>

#include "sdk/gpio.h"
#include "sdk/chipspec.h"


static struct ppsPinMapping {
    uint8_t groups;
    uint8_t setting;
};

//    |in|out|
//    DCBADCBA
const struct ppsPinMapping ppsPinMappingPins[] = {
    { 0b00000000, 0 }, // A0
    { 0b00000000, 0 }, // A1
    { 0b00000000, 0 }, // A2
    { 0b00000000, 0 }, // A3
    { 0b00000000, 0 }, // A4
    { 0b00000000, 0 }, // A5
    { 0b00000000, 0 }, // A6
    { 0b00000000, 0 }, // A7
    { 0b00000000, 0 }, // A8
    { 0b00000000, 0 }, // A9
    { 0b00000000, 0 }, // A10
    { 0b00000000, 0 }, // A11
    { 0b00000000, 0 }, // A12
    { 0b00000000, 0 }, // A13
    { 0b00010001, 0b1101 }, // A14
    { 0b00100010, 0b1101 }, // A15

    { 0b01000100, 0b0101 }, // B0
    { 0b00100010, 0b0101 }, // B1
    { 0b10001000, 0b0111 }, // B2
    { 0b00100010, 0b1000 }, // B3
    { 0b00000000, 0 }, // B4
    { 0b00010001, 0b1000 }, // B5
    { 0b10001000, 0b0101 }, // B6
    { 0b01000100, 0b0111 }, // B7
    { 0b01000100, 0b0010 }, // B8
    { 0b00010001, 0b0101 }, // B9
    { 0b00010001, 0b0110 }, // B10
    { 0b00000000, 0 }, // B11
    { 0b00000000, 0 }, // B12
    { 0b00000000, 0 }, // B13
    { 0b10001000, 0b0010 }, // B14
    { 0b01000100, 0b0011 }, // B15

    { 0b00000000, 0 }, // C0
    { 0b00010001, 0b1010 }, // C1
    { 0b10001000, 0b1100 }, // C2
    { 0b01000100, 0b1100 }, // C3
    { 0b00100010, 0b1010 }, // C4
    { 0b00000000, 0 }, // C5
    { 0b00000000, 0 }, // C6
    { 0b00000000, 0 }, // C7
    { 0b00000000, 0 }, // C8
    { 0b00000000, 0 }, // C9
    { 0b00000000, 0 }, // C10
    { 0b00000000, 0 }, // C11
    { 0b00000000, 0 }, // C12
    { 0b00100010, 0b0111 }, // C13
    { 0b00010001, 0b0111 }, // C14
    { 0b00000000, 0 }, // C15

    { 0b10001000, 0b0011 }, // D0
    { 0b10001000, 0b0000 }, // D1
    { 0b00010001, 0b0000 }, // D2
    { 0b00100010, 0b0000 }, // D3
    { 0b01000100, 0b0100 }, // D4
    { 0b10001000, 0b0110 }, // D5
    { 0b00010001, 0b1110 }, // D6
    { 0b00100010, 0b1110 }, // D7
    { 0b00000000, 0 }, // D8
    { 0b01000100, 0b0000 }, // D9
    { 0b00010001, 0b0110 }, // D10
    { 0b00100010, 0b0011 }, // D11
    { 0b01000100, 0b1010 }, // D12
    { 0b00000000, 0 }, // D13
    { 0b00010001, 0b1011 }, // D14
    { 0b00100010, 0b1011 }, // D15

    { 0b00000000, 0 }, // E0
    { 0b00000000, 0 }, // E1
    { 0b00000000, 0 }, // E2
    { 0b01000100, 0b0110 }, // E3
    { 0b00000000, 0 }, // E4
    { 0b00100010, 0b0110 }, // E5
    { 0b00000000, 0 }, // E6
    { 0b00000000, 0 }, // E7
    { 0b10001000, 0b1101 }, // E8
    { 0b01000100, 0b1101 }, // E9
    { 0b00000000, 0 }, // E10
    { 0b00000000, 0 }, // E11
    { 0b00000000, 0 }, // E12
    { 0b00000000, 0 }, // E13
    { 0b00000000, 0 }, // E14
    { 0b00000000, 0 }, // E15

    { 0b00100010, 0b0100 }, // F0
    { 0b00010001, 0b0100 }, // F1
    { 0b10001000, 0b1011 }, // F2
    { 0b10001000, 0b1000 }, // F3
    { 0b00010001, 0b0010 }, // F4
    { 0b00100010, 0b0010 }, // F5
    { 0b00000000, 0 }, // F6
    { 0b00000000, 0 }, // F7
    { 0b01000100, 0b1011 }, // F8
    { 0b00000000, 0 }, // F9
    { 0b00000000, 0 }, // F10
    { 0b00000000, 0 }, // F11
    { 0b01000100, 0b1001 }, // F12
    { 0b10001000, 0b1001 }, // F13
    { 0b00000000, 0 }, // F14
    { 0b00000000, 0 }, // F15

    { 0b00100010, 0b1100 }, // G0
    { 0b00010001, 0b1100 }, // G1
    { 0b00000000, 0 }, // G2
    { 0b00000000, 0 }, // G3
    { 0b00000000, 0 }, // G4
    { 0b00000000, 0 }, // G5
    { 0b01000100, 0b0011 }, // G6
    { 0b00100010, 0b0001 }, // G7
    { 0b00010001, 0b0001 }, // G8
    { 0b10001000, 0b0001 }, // G9
    { 0b00000000, 0 }, // G10
    { 0b00000000, 0 }, // G11
    { 0b00000000, 0 }, // G12
    { 0b00000000, 0 }, // G13
    { 0b00000000, 0 }, // G14
    { 0b00000000, 0 }, // G15
};

#define NUM_PPS_PINS (sizeof(ppsPinMappingPins) / sizeof(ppsPinMappingPins[0]))

// * - not remappable, but space in the table for them
const struct ppsPinMapping ppsPinMappingFunctions[] = {
    // 0 - 9
    { 0b00000000, 0 }, // INT0R *   1400
    { 0b10000000, 0 }, // INT1R     1404
    { 0b01000000, 0 }, // INT2R     1408
    { 0b00010000, 0 }, // INT3R     140C
    { 0b00100000, 0 }, // INT4R     1410
    { 0b00000000, 0 }, // T1CKR *   1414
    { 0b00010000, 0 }, // T2CKR     1418
    { 0b01000000, 0 }, // T3CKR     141C
    { 0b10000000, 0 }, // T4CKR     1420
    { 0b00100000, 0 }, // T5CKR     1424

    // 10 - 19
    { 0b00010000, 0 }, // T6CKR     1428
    { 0b00100000, 0 }, // T7CKR     142C
    { 0b01000000, 0 }, // T8CKR     1430
    { 0b10000000, 0 }, // T9CKR     1434
    { 0b10000000, 0 }, // IC1R      1438
    { 0b01000000, 0 }, // IC2R      143C
    { 0b00010000, 0 }, // IC3R      1440
    { 0b00100000, 0 }, // IC4R      1444
    { 0b01000000, 0 }, // IC5R      1448
    { 0b10000000, 0 }, // IC6R      144C

    // 20 - 29
    { 0b00010000, 0 }, // IC7R      1450
    { 0b00100000, 0 }, // IC8R      1454
    { 0b01000000, 0 }, // IC9R      1458
    { 0b00000000, 0 }, // ???? *    145C 
    { 0b10000000, 0 }, // OCFAR     1460
    { 0b00000000, 0 }, // ???? *    1464 
    { 0b00010000, 0 }, // U1RXR     1468
    { 0b01000000, 0 }, // U1CTSR    146C
    { 0b01000000, 0 }, // U2RXR     1470
    { 0b00010000, 0 }, // U2CTSR    1474

    // 30 - 39
    { 0b00100000, 0 }, // U3RXR     1478
    { 0b10000000, 0 }, // U3CTSR    147C
    { 0b10000000, 0 }, // U4RXR     1480
    { 0b00100000, 0 }, // U4CTSR    1484
    { 0b00010000, 0 }, // U5RXR     1488
    { 0b01000000, 0 }, // U5CTSR    148C
    { 0b10000000, 0 }, // U6RXR     1490
    { 0b00010000, 0 }, // U6CTSR    1494
    { 0b00000011, 0b0101 }, // SDO1²     1498 
    { 0b00010000, 0 }, // SDI1R     149C

    // 40 - 49
    { 0b01000100, 0b0101 }, // SS1R      14A0
    { 0b00000011, 0b0110 }, // SDO2²     14A4 
    { 0b00100000, 0 }, // SDI2R     14A8
    { 0b10001000, 0b0110 }, // SS2R      14AC
    { 0b00000011, 0b0111 }, // SDO3²     14B0 
    { 0b00010000, 0 }, // SDI3R     14B4
    { 0b01000100, 0b0111 }, // SS3R      14B8
    { 0b00001010, 0b1000 }, // SDO4²     14BC 
    { 0b00100000, 0 }, // SDI4R     14C0
    { 0b01000100, 0b1000 }, // SS4R      14C4

    // 50 - 59
    { 0b00000011, 0b1001 }, // SDO5²     14C8 
    { 0b00010000, 0 }, // SDI5R     14CC
    { 0b01000100, 0b1001 }, // SS5R      14D0
    { 0b00001100, 0b1010 }, // SDO6²     14D4
    { 0b10000000, 0 }, // SDI6R     14D8
    { 0b00010001, 0b1010 }, // SS6R      14DC
    { 0b00100000, 0 }, // C1RXR     14E0
    { 0b01000000, 0 }, // C2RXR     14E4
    { 0b00010000, 0 }, // REFCLKI1R 14E8
    { 0b00000000, 0 }, // ???? *    14EC

    // 60 - 69
    { 0b10000000, 0 }, // REFCLKI3R 14F0
    { 0b00100000, 0 }, // REFCLKI4R 14F4
    { 0b00000010, 0b0001 }, // U1TX²
    { 0b00001000, 0b0001 }, // U1RTS²
    { 0b00001000, 0b0010 }, // U2TX²
    { 0b00000010, 0b0010 }, // U2RTS²
    { 0b00000001, 0b0001 }, // U3TX²
    { 0b00000100, 0b0001 }, // U3RTS²
    { 0b00000100, 0b0010 }, // U4TX²
    { 0b00000001, 0b0010 }, // U4RTS²

    // 70 - 79
    { 0b00000010, 0b0011 }, // U5TX²
    { 0b00001000, 0b0011 }, // U5RTS²
    { 0b00001100, 0b0100 }, // U6TX²
    { 0b00000010, 0b0100 }, // U6RTS²
    { 0b00001000, 0b1100 }, // OC1²
    { 0b00001000, 0b1011 }, // OC2²
    { 0b00000001, 0b1011 }, // OC3²
    { 0b00000010, 0b1011 }, // OC4²
    { 0b00000100, 0b1011 }, // OC5²
    { 0b00000001, 0b1100 }, // OC6²

    // 80 - 89
    { 0b00000010, 0b1100 }, // OC7²
    { 0b00000100, 0b1100 }, // OC8²
    { 0b00001000, 0b1101 }, // OC9²
    { 0b00000010, 0b1111 }, // REFCLKO1²
    { 0b00000000, 0 }, // REFCLKO2²
    { 0b00000100, 0b1111 }, // REFCLKO3²
    { 0b00000001, 0b1101 }, // REFCLKO4²
    { 0b00000100, 0b1110 }, // C1OUT²
    { 0b00000001, 0b1110 }, // C2OUT²
    { 0b00000001, 0b1111 }, // C1TX²

    // 90 - 99
    { 0b00001000, 0b1111 }, // C2TX²
};
#define NUM_PPS_FUNCTIONS (sizeof(ppsPinMappingFunctions) / sizeof(ppsPinMappingFunctions[0]))

void gpio_set_mode(uint8_t pin, uint8_t mode) {
#if __CHIP_HAS_PPS

    gpio_clear_output_function(pin);

    if (mode & gpioMODE_INPUT) {
        // Switch to input mode
        *gpioPIN_TO_REGSUB(pin, TRIS, SET) = gpioPIN_TO_BIT(pin);
        *gpioPIN_TO_REGSUB(pin, ANSEL, CLR) = gpioPIN_TO_BIT(pin);

        if (mode & gpioMODE_PULLUP) {
            *gpioPIN_TO_REGSUB(pin, CNPU, SET) = gpioPIN_TO_BIT(pin);
        } else {
            *gpioPIN_TO_REGSUB(pin, CNPU, CLR) = gpioPIN_TO_BIT(pin);
        }

        if (mode & gpioMODE_PULLDOWN) {
            *gpioPIN_TO_REGSUB(pin, CNPD, SET) = gpioPIN_TO_BIT(pin);
        } else {
            *gpioPIN_TO_REGSUB(pin, CNPD, CLR) = gpioPIN_TO_BIT(pin);
        }

    } else {
        *gpioPIN_TO_REGSUB(pin, TRIS, CLR) = gpioPIN_TO_BIT(pin);

        if (mode & gpioMODE_OPENDRAIN) {
            *gpioPIN_TO_REGSUB(pin, ODC, SET) = gpioPIN_TO_BIT(pin);
        } else {
            *gpioPIN_TO_REGSUB(pin, ODC, CLR) = gpioPIN_TO_BIT(pin);
        }
    }

#else
#error "Sorry, non-PPS chips aren't surported yet"
#endif

}

uint8_t gpio_read(uint8_t pin) {
    uint16_t val = *gpioPIN_TO_REG(pin, PORT);
    if (val & gpioPIN_TO_BIT(pin)) {
        return 1;
    } else {
        return 0;
    }
}

void gpio_write(uint8_t pin, uint8_t val) {
    if (val == 0) {
        *gpioPIN_TO_REGSUB(pin, LAT, CLR) = gpioPIN_TO_BIT(pin);
    } else {
        *gpioPIN_TO_REGSUB(pin, LAT, SET) = gpioPIN_TO_BIT(pin);
    }
}

int gpio_set_input_function(uint8_t pin, uint8_t function) {
    if (pin >= NUM_PPS_PINS) return 0;
    if (function >= NUM_PPS_FUNCTIONS) return 0;
    if (((ppsPinMappingPins[pin].groups & ppsPinMappingFunctions[function].groups) & 0xF0) == 0) return 0;
    volatile uint32_t *mappingRegisters = (volatile uint32_t *)0xbf801400;
    mappingRegisters[function] = ppsPinMappingPins[pin].setting;
    return 1;
}

int gpio_set_output_function(uint8_t pin, uint8_t function) {
    if (pin >= NUM_PPS_PINS) return 0;
    if (function >= NUM_PPS_FUNCTIONS) return 0;
    if (((ppsPinMappingPins[pin].groups & ppsPinMappingFunctions[function].groups) & 0x0F) == 0) return 0;
    volatile uint32_t *mappingRegisters = (volatile uint32_t *)0xbf801500;
    mappingRegisters[pin] = ppsPinMappingFunctions[function].setting;
//if ( == gpioF5) gpio_write(gpioE6, 1);
    return 1;
}

int gpio_clear_output_function(uint8_t pin) {
    if (pin >= NUM_PPS_PINS) return 0;
    if ((ppsPinMappingPins[pin].groups & 0x0F) != 0) {
        volatile uint32_t *pinMappingRegisters = (volatile uint32_t *)0xbf801500;
        pinMappingRegisters[pin] = 0;
        return 1;
    }
    return 0;
}

void gpio_unlock_pps() {
}

void gpio_lock_pps() {
}
