#ifndef _SDK_GPIO_H
#define _SDK_GPIO_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void gpio_set_mode(uint8_t, uint8_t);
extern void gpio_write(uint8_t, uint8_t);
extern uint8_t gpio_read(uint8_t);
extern int gpio_set_input_function(uint8_t pin, uint8_t function);
extern int gpio_set_output_function(uint8_t pin, uint8_t function);
extern int gpio_clear_output_function(uint8_t pin);


#ifdef __cplusplus
}
#endif

typedef volatile uint32_t *sfr_t;

#define gpioPIN_TO_REGSUB(P, R, S)  ((sfr_t)(&R ## B ## S + (((P)/16 * 0x40)) - 0x40))
#define gpioPIN_TO_REG(P, R)        ((sfr_t)(&R ## B + (((P)/16 * 0x40)) - 0x40))
#define gpioPIN_TO_OFFSET(P)        ((P) & 0x0F)
#define gpioPIN_TO_BIT(P)           (1 << ((P) & 0x0F))
#define gpioPIN_TO_MASK(P)          (~(1 << ((P) & 0x0F)))

#define gpioMODE_OUTPUT         0x00
#define gpioMODE_INPUT          0x01
#define gpioMODE_OPENDRAIN      0x02
#define gpioMODE_PULLUP         0x04
#define gpioMODE_PULLDOWN       0x08

#define PPS_MODE_GPIO           0

#define gpioA0                  0x00
#define gpioA1                  0x01
#define gpioA2                  0x02
#define gpioA3                  0x03
#define gpioA4                  0x04
#define gpioA5                  0x05
#define gpioA6                  0x06
#define gpioA7                  0x07
#define gpioA8                  0x08
#define gpioA9                  0x09
#define gpioA10                 0x0A
#define gpioA11                 0x0B
#define gpioA12                 0x0C
#define gpioA13                 0x0D
#define gpioA14                 0x0E
#define gpioA15                 0x0F

#define gpioB0                  0x10
#define gpioB1                  0x11
#define gpioB2                  0x12
#define gpioB3                  0x13
#define gpioB4                  0x14
#define gpioB5                  0x15
#define gpioB6                  0x16
#define gpioB7                  0x17
#define gpioB8                  0x18
#define gpioB9                  0x19
#define gpioB10                 0x1A
#define gpioB11                 0x1B
#define gpioB12                 0x1C
#define gpioB13                 0x1D
#define gpioB14                 0x1E
#define gpioB15                 0x1F

#define gpioC0                  0x20
#define gpioC1                  0x21
#define gpioC2                  0x22
#define gpioC3                  0x23
#define gpioC4                  0x24
#define gpioC5                  0x25
#define gpioC6                  0x26
#define gpioC7                  0x27
#define gpioC8                  0x28
#define gpioC9                  0x29
#define gpioC10                 0x2A
#define gpioC11                 0x2B
#define gpioC12                 0x2C
#define gpioC13                 0x2D
#define gpioC14                 0x2E
#define gpioC15                 0x2F

#define gpioD0                  0x30
#define gpioD1                  0x31
#define gpioD2                  0x32
#define gpioD3                  0x33
#define gpioD4                  0x34
#define gpioD5                  0x35
#define gpioD6                  0x36
#define gpioD7                  0x37
#define gpioD8                  0x38
#define gpioD9                  0x39
#define gpioD10                 0x3A
#define gpioD11                 0x3B
#define gpioD12                 0x3C
#define gpioD13                 0x3D
#define gpioD14                 0x3E
#define gpioD15                 0x3F

#define gpioE0                  0x40
#define gpioE1                  0x41
#define gpioE2                  0x42
#define gpioE3                  0x43
#define gpioE4                  0x44
#define gpioE5                  0x45
#define gpioE6                  0x46
#define gpioE7                  0x47
#define gpioE8                  0x48
#define gpioE9                  0x49
#define gpioE10                 0x4A
#define gpioE11                 0x4B
#define gpioE12                 0x4C
#define gpioE13                 0x4D
#define gpioE14                 0x4E
#define gpioE15                 0x4F

#define gpioF0                  0x50
#define gpioF1                  0x51
#define gpioF2                  0x52
#define gpioF3                  0x53
#define gpioF4                  0x54
#define gpioF5                  0x55
#define gpioF6                  0x56
#define gpioF7                  0x57
#define gpioF8                  0x58
#define gpioF9                  0x59
#define gpioF10                 0x5A
#define gpioF11                 0x5B
#define gpioF12                 0x5C
#define gpioF13                 0x5D
#define gpioF14                 0x5E
#define gpioF15                 0x5F

#define gpioG0                  0x60
#define gpioG1                  0x61
#define gpioG2                  0x62
#define gpioG3                  0x63
#define gpioG4                  0x64
#define gpioG5                  0x65
#define gpioG6                  0x66
#define gpioG7                  0x67
#define gpioG8                  0x68
#define gpioG9                  0x69
#define gpioG10                 0x6A
#define gpioG11                 0x6B
#define gpioG12                 0x6C
#define gpioG13                 0x6D
#define gpioG14                 0x6E
#define gpioG15                 0x6F

#define gpioH0                  0x70
#define gpioH1                  0x71
#define gpioH2                  0x72
#define gpioH3                  0x73
#define gpioH4                  0x74
#define gpioH5                  0x75
#define gpioH6                  0x76
#define gpioH7                  0x77
#define gpioH8                  0x78
#define gpioH9                  0x79
#define gpioH10                 0x7A
#define gpioH11                 0x7B
#define gpioH12                 0x7C
#define gpioH13                 0x7D
#define gpioH14                 0x7E
#define gpioH15                 0x7F

#define gpioI0                  0x80
#define gpioI1                  0x81
#define gpioI2                  0x82
#define gpioI3                  0x83
#define gpioI4                  0x84
#define gpioI5                  0x85
#define gpioI6                  0x86
#define gpioI7                  0x87
#define gpioI8                  0x88
#define gpioI9                  0x89
#define gpioI10                 0x8A
#define gpioI11                 0x8B
#define gpioI12                 0x8C
#define gpioI13                 0x8D
#define gpioI14                 0x8E
#define gpioI15                 0x8F

#define gpioJ0                  0x90
#define gpioJ1                  0x91
#define gpioJ2                  0x92
#define gpioJ3                  0x93
#define gpioJ4                  0x94
#define gpioJ5                  0x95
#define gpioJ6                  0x96
#define gpioJ7                  0x97
#define gpioJ8                  0x98
#define gpioJ9                  0x99
#define gpioJ10                 0x9A
#define gpioJ11                 0x9B
#define gpioJ12                 0x9C
#define gpioJ13                 0x9D
#define gpioJ14                 0x9E
#define gpioJ15                 0x9F


#define gpioPPS_INT0       0
#define gpioPPS_INT1       1
#define gpioPPS_INT2       2
#define gpioPPS_INT3       3
#define gpioPPS_INT4       4
#define gpioPPS_T2CK       6
#define gpioPPS_T3CK       7
#define gpioPPS_T4CK       8
#define gpioPPS_T5CK       9
#define gpioPPS_T6CK       10
#define gpioPPS_T7CK       11
#define gpioPPS_T8CK       12
#define gpioPPS_T9CK       13
#define gpioPPS_IC1        14
#define gpioPPS_IC2        15
#define gpioPPS_IC3        16
#define gpioPPS_IC4        17
#define gpioPPS_IC5        18
#define gpioPPS_IC6        19
#define gpioPPS_IC7        20
#define gpioPPS_IC8        21
#define gpioPPS_IC9        22
#define gpioPPS_OCFA       24
#define gpioPPS_U1RX       26
#define gpioPPS_U1CTS      27
#define gpioPPS_U2RX       28
#define gpioPPS_U2CTS      29
#define gpioPPS_U3RX       30
#define gpioPPS_U3CTS      31
#define gpioPPS_U4RX       32
#define gpioPPS_U4CTS      33
#define gpioPPS_U5RX       34
#define gpioPPS_U5CTS      35
#define gpioPPS_U6RX       36
#define gpioPPS_U6CTS      37
#define gpioPPS_SDO1       38
#define gpioPPS_SDI1       39
#define gpioPPS_SS1        40
#define gpioPPS_SDO2       41
#define gpioPPS_SDI2       42
#define gpioPPS_SS2        43
#define gpioPPS_SDO3       44
#define gpioPPS_SDI3       45
#define gpioPPS_SS3        46
#define gpioPPS_SDO4       47
#define gpioPPS_SDI4       48
#define gpioPPS_SS4        49
#define gpioPPS_SDO5       50
#define gpioPPS_SDI5       51
#define gpioPPS_SS5        52
#define gpioPPS_SDO6       53
#define gpioPPS_SDI6       54
#define gpioPPS_SS6        55
#define gpioPPS_C1RX       56
#define gpioPPS_C2RX       57
#define gpioPPS_REFCLKI1   58
#define gpioPPS_REFCLKI3   60
#define gpioPPS_REFCLKI4   61
#define gpioPPS_U1TX       62
#define gpioPPS_U1RTS      63
#define gpioPPS_U2TX       64
#define gpioPPS_U2RTS      65
#define gpioPPS_U3TX       66
#define gpioPPS_U3RTS      67
#define gpioPPS_U4TX       68
#define gpioPPS_U4RTS      69
#define gpioPPS_U5TX       70
#define gpioPPS_U5RTS      71
#define gpioPPS_U6TX       72
#define gpioPPS_U6RTS      73
#define gpioPPS_OC1        74
#define gpioPPS_OC2        75
#define gpioPPS_OC3        76
#define gpioPPS_OC4        77
#define gpioPPS_OC5        78
#define gpioPPS_OC6        79
#define gpioPPS_OC7        80
#define gpioPPS_OC8        81
#define gpioPPS_OC9        82
#define gpioPPS_REFCLKO1   83
#define gpioPPS_REFCLKO2   84
#define gpioPPS_REFCLKO3   85
#define gpioPPS_REFCLKO4   86
#define gpioPPS_C1OUT      87
#define gpioPPS_C2OUT      88
#define gpioPPS_C1TX       89
#define gpioPPS_C2TX       90

#endif
