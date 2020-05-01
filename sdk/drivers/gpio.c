/**
 * @file gpio.c
 * Routines for controlling GPIO pins in digital modes. This also includes
 * external and change notification interrupts.
 */
#include <p32xxxx.h>
#include <sys/attribs.h>

#include "sdk/gpio.h"
#include "sdk/chipspec.h"

static struct cnInterruptCallback {
    gpioISR_t fallingEdge;
    gpioISR_t risingEdge;
};

static struct cnInterruptCallback cnInterruptPinCallback[__CHIP_MAX_GPIO + 1] = {0};
static void (*externalInterrupt[5])() = {0};

static struct ppsPinMapping {
    uint8_t groups;
    uint8_t setting;
};

static const struct ppsPinMapping ppsPinMappingPins[] = {
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
static const struct ppsPinMapping ppsPinMappingFunctions[] = {
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

/**
 * Set the IO mode for a GPIO pin
 *
 * Configures a pin to be either input or output, and controls extra facilities
 * such as open-drain and pullup/down resistors.
 * 
 * A pin mode is specified as a bitmap of a combination of macros to build the
 * completed mode. One of gpioMODE_OUTPUT or gpioMODE_INPUT must be specified.
 * for gpioMODE_OUTPUT you may also combine it with gpioMODE_OPENDRAIN. For 
 * gpioMODE_INPUT you may also combine it with gpioMODE_PULLUP and gpioMODE_PULLDOWN.
 *
 * @param pin The GPIO pin index
 * @param mode The IO mode to set
 */
void gpio_set_mode(uint8_t pin, uint8_t mode) {
    if (pin > __CHIP_MAX_GPIO) return;
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

/**
 * Read the state of a GPIO pin
 * @param pin The pin index to read
 * @returns 1 if the input is HIGH or 0 if LOW
 */
uint8_t gpio_read(uint8_t pin) {
    if (pin > __CHIP_MAX_GPIO) return 0;
    uint16_t val = *gpioPIN_TO_REG(pin, PORT);
    if (val & gpioPIN_TO_BIT(pin)) {
        return 1;
    } else {
        return 0;
    }
}

/**
 * Set the output level of a GPIO pin
 * @param pin The pin index to write
 * @param val 1 to drive the pin HIGH or 0 to drive it LOW
 */
void gpio_write(uint8_t pin, uint8_t val) {
    if (pin > __CHIP_MAX_GPIO) return;
    if (val == 0) {
        *gpioPIN_TO_REGSUB(pin, LAT, CLR) = gpioPIN_TO_BIT(pin);
    } else {
        *gpioPIN_TO_REGSUB(pin, LAT, SET) = gpioPIN_TO_BIT(pin);
    }
}

/** 
 * Configure the input Peripheral Pin Select function on a pin
 * @param pin The pin to configure
 * @param function The PPS function to assign in the form gpioPPS_<FUNC>
 * @returns 1 if the function could be assigned, 0 otherwise
 */
int gpio_set_input_function(uint8_t pin, uint8_t function) {
    if (pin > __CHIP_MAX_GPIO) return 0;
    if (pin >= NUM_PPS_PINS) return 0;
    if (function >= NUM_PPS_FUNCTIONS) return 0;
    if (((ppsPinMappingPins[pin].groups & ppsPinMappingFunctions[function].groups) & 0xF0) == 0) return 0;
    volatile uint32_t *mappingRegisters = (volatile uint32_t *)0xbf801400;
    mappingRegisters[function] = ppsPinMappingPins[pin].setting;
    return 1;
}

/**
 * Configure the output Peripheral Pin Select function on a pin
 * @param pin The pin to configure
 * @param function The PPS function to assign in the form gpioPPS_<FUNC>
 * @returns 1 if the function could be assigned, 0 otherwise
 */
int gpio_set_output_function(uint8_t pin, uint8_t function) {
    if (pin > __CHIP_MAX_GPIO) return 0;
    if (pin >= NUM_PPS_PINS) return 0;
    if (function >= NUM_PPS_FUNCTIONS) return 0;
    if (((ppsPinMappingPins[pin].groups & ppsPinMappingFunctions[function].groups) & 0x0F) == 0) return 0;
    volatile uint32_t *mappingRegisters = (volatile uint32_t *)0xbf801500;
    mappingRegisters[pin] = ppsPinMappingFunctions[function].setting;
    return 1;
}

/**
 * Remove the output Peripheral Pin Select function from a pin returning it to
 * normal GPIO usage
 * @param pin The pin to configur
 * @returns 1 if the function could be removed, 0 otherwise
 */
int gpio_clear_output_function(uint8_t pin) {
    if (pin > __CHIP_MAX_GPIO) return 0;
    if (pin >= NUM_PPS_PINS) return 0;
    if ((ppsPinMappingPins[pin].groups & 0x0F) != 0) {
        volatile uint32_t *pinMappingRegisters = (volatile uint32_t *)0xbf801500;
        pinMappingRegisters[pin] = 0;
        return 1;
    }
    return 0;
}

/**
 * Connect the Change Notfication interrupt of a GPIO pin to a callback routine. Different
 * callback routines can be connected to both the rising edge and falling edge interrupts.
 * @param pin The pin to attach the callback to
 * @param type The edge to connect to. One of:
 * * gpioINTERRUPT_RISING
 * * gpioINTERRUPT_FALLING
 * @param callback The function to execute when the interrupt triggers. Two parameters
 *                 passed (both uint8_t). The first is the pin number the interrupt
 *                 occurred on, the second is the pin state 0 or 1.
 * @returns 1 if the interrupt could be configured and connected, 0 otherwise
 */
int gpio_connect_change_interrupt(uint8_t pin, uint8_t type, gpioISR_t callback) {
    if (pin > __CHIP_MAX_GPIO) return 0;

    if (type == gpioINTERRUPT_FALLING) {
        cnInterruptPinCallback[pin].fallingEdge = callback;
    } else if (type == gpioINTERRUPT_RISING) {
        cnInterruptPinCallback[pin].risingEdge = callback;
    } else {
        return 0;
    }

    *gpioPIN_TO_REGSUB(pin, CNEN, SET) = gpioPIN_TO_BIT(pin);

    (void)gpio_read(pin);

    int vector = 118 + (pin >> 4);
    if (cpu_get_interrupt_enable(vector) == 0) {
        *gpioPIN_TO_REGSUB(pin, CNCON, CLR) = (1 << 11); // EDGEDETECT
        *gpioPIN_TO_REGSUB(pin, CNCON, SET) = (1 << 15); // ON
        cpu_set_interrupt_priority(vector, 6, 0);
        cpu_clear_interrupt_flag(vector);
        cpu_set_interrupt_enable(vector);
    }

    return 1;
}

/**
 * Disconnect a Change Notification callback from a pin
 * @param pin The pin index to disconnect the callback from
 * @param type The edge to disconnect from. One of:
 * * gpioINTERRUPT_RISING
 * * gpioINTERRUPT_FALLING
 * @returns 1 if the interrupt could be disconnected, 0 otherwise
 */ 
int gpio_disconnect_change_interrupt(uint8_t pin, uint8_t type) {
    if (pin > __CHIP_MAX_GPIO) return 0; 

    if (type == gpioINTERRUPT_FALLING) {
        cnInterruptPinCallback[pin].fallingEdge = NULL;
        *gpioPIN_TO_REGSUB(pin, CNNE, CLR) = gpioPIN_TO_BIT(pin);
    } else if (type == gpioINTERRUPT_RISING) { 
        cnInterruptPinCallback[pin].risingEdge = NULL;
        *gpioPIN_TO_REGSUB(pin, CNEN, CLR) = gpioPIN_TO_BIT(pin);
    } else {
        return 0;
    }

    if ((cnInterruptPinCallback[pin].fallingEdge == NULL) && (cnInterruptPinCallback[pin].risingEdge == NULL)) {

        uint16_t cnen = *gpioPIN_TO_REG(pin, CNEN);

        if (cnen == 0) {
            int vector = 118 + (pin >> 4);
            if (cpu_get_interrupt_enable(vector) == 1) { 
                *gpioPIN_TO_REGSUB(pin, CNCON, CLR) = (1 << 11); // EDGEDETECT
                *gpioPIN_TO_REGSUB(pin, CNCON, CLR) = (1 << 15); // ON
                cpu_clear_interrupt_enable(vector);
            } 
        }
    }
    return 1;
}

/**
 * Connect an interrupt routine to an external interrupt. Unlike Change Notification interrupts
 * only one edge of an external interrupt can be triggered on. However they are more responsive
 * that change notification.
 * @param pin The pin to configure the interrupt on through PPS (if needed)
 * @param interrupt The interrupt number (0-4) to connect to
 * @param mode The edge to connect to. One of:
 * * gpioINTERRUPT_RISING
 * * gpioINTERRUPT_FALLING
 * @param callback The callback to connect with the interrupt. void callback()
 * @returns 1 if the interrupt could be configured and connected, 0 otherwise
 */
int gpio_connect_external_interrupt(uint8_t pin, uint8_t interrupt, uint8_t mode, void (*callback)()) {
    int ret = 0;
    switch (interrupt) {
        case 0:
            ret = 1; // Nothing to connect for INT0
            break;
        case 1:
            ret = gpio_set_input_function(pin, gpioPPS_INT1);
            break;
        case 2:
            ret = gpio_set_input_function(pin, gpioPPS_INT2);
            break;
        case 3:
            ret = gpio_set_input_function(pin, gpioPPS_INT3);
            break;
        case 4:
            ret = gpio_set_input_function(pin, gpioPPS_INT4);
            break;
    }
    if (ret == 0) return 0;
 
    externalInterrupt[interrupt] = callback;

    if (mode == gpioINTERRUPT_RISING) {
        INTCONSET = 1<<interrupt;
    } else {
        INTCONCLR = 1<<0;
    }
    cpu_set_interrupt_priority(_EXTERNAL_0_VECTOR + interrupt, 6, 0);
    cpu_clear_interrupt_flag(_EXTERNAL_0_VECTOR + interrupt);
    cpu_set_interrupt_enable(_EXTERNAL_0_VECTOR + interrupt);
}

/**
 * Disconnect an external interrupt callback from a pin
 * @param pin The pin index to disconnect
 * @returns 1 if the interrupt could be disconnected, 0 otherwise.
 */
int gpio_disconnect_external_interrupt(uint8_t interrupt) {
    if (interrupt > 4) return 0;
    cpu_clear_interrupt_enable(_EXTERNAL_0_VECTOR + interrupt);
    cpu_set_interrupt_priority(_EXTERNAL_0_VECTOR + interrupt, 0, 0);
    externalInterrupt[interrupt] = NULL;
    return 1;
}


void __ISR(_EXTERNAL_0_VECTOR, IPL6) gpio_ext_0() {
    cpu_clear_interrupt_flag(_EXTERNAL_0_VECTOR);
    if (externalInterrupt[0] != NULL) externalInterrupt[0]();
}

void __ISR(_EXTERNAL_1_VECTOR, IPL6) gpio_ext_1() {
    cpu_clear_interrupt_flag(_EXTERNAL_1_VECTOR);
    if (externalInterrupt[1] != NULL) externalInterrupt[1]();
}

void __ISR(_EXTERNAL_2_VECTOR, IPL6) gpio_ext_2() {
    cpu_clear_interrupt_flag(_EXTERNAL_2_VECTOR);
    if (externalInterrupt[2] != NULL) externalInterrupt[2]();
}

void __ISR(_EXTERNAL_3_VECTOR, IPL6) gpio_ext_3() {
    cpu_clear_interrupt_flag(_EXTERNAL_3_VECTOR);
    if (externalInterrupt[3] != NULL) externalInterrupt[3]();
}

void __ISR(_EXTERNAL_4_VECTOR, IPL6) gpio_ext_4() {
    cpu_clear_interrupt_flag(_EXTERNAL_4_VECTOR);
    if (externalInterrupt[4] != NULL) externalInterrupt[4]();
}

#if defined(_CHANGE_NOTICE_A_VECTOR)
void __ISR(_CHANGE_NOTICE_A_VECTOR, IPL6) gpio_cn_a() {
    static uint32_t storedState = 0;
    cpu_clear_interrupt_flag(_CHANGE_NOTICE_A_VECTOR);

    uint32_t currentState = PORTA & CNENA;
    uint32_t changes = storedState ^ currentState;

    storedState = currentState;

    for (int i = 0; i < 16; i++) {
        uint32_t bit = 1 << i;
        if ((changes & bit) != 0) {
            if (((currentState & bit) != 0) && (cnInterruptPinCallback[0x00 + i].risingEdge != NULL)) {
                cnInterruptPinCallback[0x00 + i].risingEdge(0x00 + i, 1);
            } else if (((currentState & bit) == 0) && (cnInterruptPinCallback[0x00 + i].fallingEdge != NULL)) {
                cnInterruptPinCallback[0x00 + i].fallingEdge(0x00 + i, 0);
            }
        }
        bit <<= 1;
    }
}
#endif

#if defined(_CHANGE_NOTICE_B_VECTOR)
void __ISR(_CHANGE_NOTICE_B_VECTOR, IPL6) gpio_cn_b() {
    static uint32_t storedState = 0;
    cpu_clear_interrupt_flag(_CHANGE_NOTICE_B_VECTOR);

    uint32_t currentState = PORTB & CNENB;
    uint32_t changes = storedState ^ currentState;

    storedState = currentState;

    for (int i = 0; i < 16; i++) {
        uint32_t bit = 1 << i;
        if ((changes & bit) != 0) {
            if (((currentState & bit) != 0) && (cnInterruptPinCallback[0x10 + i].risingEdge != NULL)) {
                cnInterruptPinCallback[0x10 + i].risingEdge(0x10 + i, 1);
            } else if (((currentState & bit) == 0) && (cnInterruptPinCallback[0x10 + i].fallingEdge != NULL)) {
                cnInterruptPinCallback[0x10 + i].fallingEdge(0x10 + i, 0);
            }
        }
        bit <<= 1;
    }
}
#endif

#if defined(_CHANGE_NOTICE_C_VECTOR)
void __ISR(_CHANGE_NOTICE_C_VECTOR, IPL6) gpio_cn_c() {
    static uint32_t storedState = 0;
    cpu_clear_interrupt_flag(_CHANGE_NOTICE_C_VECTOR);

    uint32_t currentState = PORTC & CNENC;
    uint32_t changes = storedState ^ currentState;

    storedState = currentState;

    for (int i = 0; i < 16; i++) {
        uint32_t bit = 1 << i;
        if ((changes & bit) != 0) {
            if (((currentState & bit) != 0) && (cnInterruptPinCallback[0x20 + i].risingEdge != NULL)) {
                cnInterruptPinCallback[0x20 + i].risingEdge(0x20 + i, 1);
            } else if (((currentState & bit) == 0) && (cnInterruptPinCallback[0x20 + i].fallingEdge != NULL)) {
                cnInterruptPinCallback[0x20 + i].fallingEdge(0x20 + i, 0);
            }
        }
        bit <<= 1;
    }
}
#endif

#if defined(_CHANGE_NOTICE_D_VECTOR)
void __ISR(_CHANGE_NOTICE_D_VECTOR, IPL6) gpio_cn_d() {
    static uint32_t storedState = 0;
    cpu_clear_interrupt_flag(_CHANGE_NOTICE_D_VECTOR);

    uint32_t currentState = PORTD & CNEND;
    uint32_t changes = storedState ^ currentState;

    storedState = currentState;

    for (int i = 0; i < 16; i++) {
        uint32_t bit = 1 << i;
        if ((changes & bit) != 0) {
            if (((currentState & bit) != 0) && (cnInterruptPinCallback[0x30 + i].risingEdge != NULL)) {
                cnInterruptPinCallback[0x30 + i].risingEdge(0x30 + i, 1);
            } else if (((currentState & bit) == 0) && (cnInterruptPinCallback[0x30 + i].fallingEdge != NULL)) {
                cnInterruptPinCallback[0x30 + i].fallingEdge(0x30 + i, 0);
            }
        }
        bit <<= 1;
    }
}
#endif

#if defined(_CHANGE_NOTICE_E_VECTOR)
void __ISR(_CHANGE_NOTICE_E_VECTOR, IPL6) gpio_cn_e() {
    static uint32_t storedState = 0;
    cpu_clear_interrupt_flag(_CHANGE_NOTICE_E_VECTOR);

    uint32_t currentState = PORTE & CNENE;
    uint32_t changes = storedState ^ currentState;

    storedState = currentState;

    for (int i = 0; i < 16; i++) {
        uint32_t bit = 1 << i;
        if ((changes & bit) != 0) {
            if (((currentState & bit) != 0) && (cnInterruptPinCallback[0x40 + i].risingEdge != NULL)) {
                cnInterruptPinCallback[0x40 + i].risingEdge(0x40 + i, 1);
            } else if (((currentState & bit) == 0) && (cnInterruptPinCallback[0x40 + i].fallingEdge != NULL)) {
                cnInterruptPinCallback[0x40 + i].fallingEdge(0x40 + i, 0);
            }
        }
        bit <<= 1;
    }
}
#endif

#if defined(_CHANGE_NOTICE_F_VECTOR)
void __ISR(_CHANGE_NOTICE_F_VECTOR, IPL6) gpio_cn_f() {
    static uint32_t storedState = 0;
    cpu_clear_interrupt_flag(_CHANGE_NOTICE_F_VECTOR);

    uint32_t currentState = PORTF & CNENF;
    uint32_t changes = storedState ^ currentState;

    storedState = currentState;

    for (int i = 0; i < 16; i++) {
        uint32_t bit = 1 << i;
        if ((changes & bit) != 0) {
            if (((currentState & bit) != 0) && (cnInterruptPinCallback[0x50 + i].risingEdge != NULL)) {
                cnInterruptPinCallback[0x50 + i].risingEdge(0x50 + i, 1);
            } else if (((currentState & bit) == 0) && (cnInterruptPinCallback[0x50 + i].fallingEdge != NULL)) {
                cnInterruptPinCallback[0x50 + i].fallingEdge(0x50 + i, 0);
            }
        }
        bit <<= 1;
    }
}
#endif

#if defined(_CHANGE_NOTICE_G_VECTOR)
void __ISR(_CHANGE_NOTICE_G_VECTOR, IPL6) gpio_cn_g() {
    static uint32_t storedState = 0;
    cpu_clear_interrupt_flag(_CHANGE_NOTICE_G_VECTOR);

    uint32_t currentState = PORTG & CNENG;
    uint32_t changes = storedState ^ currentState;

    storedState = currentState;

    for (int i = 0; i < 16; i++) {
        uint32_t bit = 1 << i;
        if ((changes & bit) != 0) {
            if (((currentState & bit) != 0) && (cnInterruptPinCallback[0x60 + i].risingEdge != NULL)) {
                cnInterruptPinCallback[0x60 + i].risingEdge(0x60 + i, 1);
            } else if (((currentState & bit) == 0) && (cnInterruptPinCallback[0x60 + i].fallingEdge != NULL)) {
                cnInterruptPinCallback[0x60 + i].fallingEdge(0x60 + i, 0);
            }
        }
        bit <<= 1;
    }
}
#endif

#if defined(_CHANGE_NOTICE_H_VECTOR)
void __ISR(_CHANGE_NOTICE_H_VECTOR, IPL6) gpio_cn_h() {
    static uint32_t storedState = 0;
    cpu_clear_interrupt_flag(_CHANGE_NOTICE_H_VECTOR);

    uint32_t currentState = PORTH & CNENH;
    uint32_t changes = storedState ^ currentState;

    storedState = currentState;

    for (int i = 0; i < 16; i++) {
        uint32_t bit = 1 << i;
        if ((changes & bit) != 0) {
            if (((currentState & bit) != 0) && (cnInterruptPinCallback[0x70 + i].risingEdge != NULL)) {
                cnInterruptPinCallback[0x70 + i].risingEdge(0x70 + i, 1);
            } else if (((currentState & bit) == 0) && (cnInterruptPinCallback[0x70 + i].fallingEdge != NULL)) {
                cnInterruptPinCallback[0x70 + i].fallingEdge(0x70 + i, 0);
            }
        }
        bit <<= 1;
    }
}
#endif

#if defined(_CHANGE_NOTICE_J_VECTOR)
void __ISR(_CHANGE_NOTICE_J_VECTOR, IPL6) gpio_cn_j() {
    static uint32_t storedState = 0;
    cpu_clear_interrupt_flag(_CHANGE_NOTICE_J_VECTOR);

    uint32_t currentState = PORTJ & CNENJ;
    uint32_t changes = storedState ^ currentState;

    storedState = currentState;

    for (int i = 0; i < 16; i++) {
        uint32_t bit = 1 << i;
        if ((changes & bit) != 0) {
            if (((currentState & bit) != 0) && (cnInterruptPinCallback[0x80 + i].risingEdge != NULL)) {
                cnInterruptPinCallback[0x80 + i].risingEdge(0x80 + i, 1);
            } else if (((currentState & bit) == 0) && (cnInterruptPinCallback[0x80 + i].fallingEdge != NULL)) {
                cnInterruptPinCallback[0x80 + i].fallingEdge(0x80 + i, 0);
            }
        }
        bit <<= 1;
    }
}
#endif

#if defined(_CHANGE_NOTICE_K_VECTOR)
void __ISR(_CHANGE_NOTICE_K_VECTOR, IPL6) gpio_cn_k() {
    static uint32_t storedState = 0;
    cpu_clear_interrupt_flag(_CHANGE_NOTICE_K_VECTOR);

    uint32_t currentState = PORTK & CNENK;
    uint32_t changes = storedState ^ currentState;

    storedState = currentState;

    for (int i = 0; i < 16; i++) {
        uint32_t bit = 1 << i;
        if ((changes & bit) != 0) {
            if (((currentState & bit) != 0) && (cnInterruptPinCallback[0x90 + i].risingEdge != NULL)) {
                cnInterruptPinCallback[0x90 + i].risingEdge(0x90 + i, 1);
            } else if (((currentState & bit) == 0) && (cnInterruptPinCallback[0x90 + i].fallingEdge != NULL)) {
                cnInterruptPinCallback[0x90 + i].fallingEdge(0x90 + i, 0);
            }
        }
        bit <<= 1;
    }
}
#endif

