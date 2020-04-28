#include <stdio.h>
#include <sys/attribs.h>

#include "FreeRTOS.h"
#include "task.h"
#include "stream_buffer.h"

#include "sdk/gpio.h"
#include "sdk/uart.h"
#include "sdk/chipspec.h"
#include "sdk/cpu.h"

static struct uartControlDataStruct {
    TaskHandle_t task;
    StreamBufferHandle_t txBuffer;
    StreamBufferHandle_t rxBuffer;
    const char *name;
    uint8_t txVector;
    uint8_t rxVector;
    uint8_t faultVector;
};

static struct uartControlDataStruct uartControlData[__CHIP_HAS_UART] = {
#if (__CHIP_HAS_UART > 0)
    { NULL, NULL, NULL, "UART0", _UART1_TX_VECTOR, _UART1_RX_VECTOR, _UART1_FAULT_VECTOR },
#endif
#if (__CHIP_HAS_UART > 1)
    { NULL, NULL, NULL, "UART1", _UART2_TX_VECTOR, _UART2_RX_VECTOR, _UART2_FAULT_VECTOR },
#endif
#if (__CHIP_HAS_UART > 2)
    { NULL, NULL, NULL, "UART2", _UART3_TX_VECTOR, _UART3_RX_VECTOR, _UART3_FAULT_VECTOR },
#endif
#if (__CHIP_HAS_UART > 3)
    { NULL, NULL, NULL, "UART3", _UART4_TX_VECTOR, _UART4_RX_VECTOR, _UART4_FAULT_VECTOR },
#endif
#if (__CHIP_HAS_UART > 4)
    { NULL, NULL, NULL, "UART4", _UART5_TX_VECTOR, _UART5_RX_VECTOR, _UART5_FAULT_VECTOR },
#endif
#if (__CHIP_HAS_UART > 5)
    { NULL, NULL, NULL, "UART5", _UART6_TX_VECTOR, _UART6_RX_VECTOR, _UART6_FAULT_VECTOR },
#endif
};

int uart_write(uint8_t uart, uint8_t byte) {
    xStreamBufferSend(uartControlData[uart].txBuffer, &byte, 1, portMAX_DELAY);
    xTaskNotify(uartControlData[uart].task, 0, eNoAction);
}

int uart_read(uint8_t uart) {
    if (xStreamBufferBytesAvailable(uartControlData[uart].rxBuffer) == 0) {
        return -1;
    }
    uint8_t b = 0;
    xStreamBufferReceive(uartControlData[uart].rxBuffer, &b, 1, 0);
    return b;
}

/** 
 * Configure the TX pin of the selected UART through PPS
 * @param uart The index of the UART
 * @param pin The index of the pin to assign the TX function to
 * @returns 1 on success, 0 on failure
 */
int uart_set_tx_pin(uint8_t uart, uint8_t pin) {
    if (uart >= __CHIP_HAS_UART) return 0;
//    gpio_pps_map_out(pin, uart[n].tx)
}

/** 
 * Configure the RX pin of the selected UART through PPS
 * @param uart The index of the UART
 * @param pin The index of the pin to assign the RX function to
 * @returns 1 on success, 0 on failure
 */
int uart_set_rx_pin(uint8_t uart, uint8_t pin) {
    if (uart >= __CHIP_HAS_UART) return 0;
//    gpio_pps_map_in(pin, uart[n].rx)
}

static inline uint32_t uart_calculate_baud(uint32_t baud, uint8_t *highspeed) {
    uint32_t brg;
    if (baud < 200000) {
        brg = ((cpu_get_peripheral_clock() / 16 / baud) - 1);
        highspeed = 1;
    } else {
        brg = ((cpu_get_peripheral_clock() / 4 / baud) - 1);
        highspeed = 1;
    }
}

/**
 * Configure the baud rate of the selected UART
 * @param uart The index of the UART
 * @param baud The baud rate to configure.
 * @returns 1 on success, 0 on failure
 */
int uart_set_baud(uint8_t uart, uint32_t baud) {
    if (uart >= __CHIP_HAS_UART) return 0;
#if (__CHIP_HAS_UART > 0)
    if (uart == 0) {
        uint8_t hs = 0;
        U1BRG = uart_calculate_baud(baud, &hs);
        if (hs) {
            U1MODESET = _U1MODE_BRGH_MASK;
        } else {
            U1MODECLR = _U1MODE_BRGH_MASK;
        }
        return 1;
    }
#endif
#if (__CHIP_HAS_UART > 1)
    if (uart == 1) {
        uint8_t hs = 0;
        U2BRG = uart_calculate_baud(baud, &hs);
        if (hs) {
            U2MODESET = _U2MODE_BRGH_MASK;
        } else {
            U2MODECLR = _U2MODE_BRGH_MASK;
        }
        return 1;
    }
#endif
#if (__CHIP_HAS_UART > 2)
    if (uart == 2) {
        uint8_t hs = 0;
        U3BRG = uart_calculate_baud(baud, &hs);
        if (hs) {
            U3MODESET = _U3MODE_BRGH_MASK;
        } else {
            U3MODECLR = _U3MODE_BRGH_MASK;
        }
        return 1;
    }
#endif
#if (__CHIP_HAS_UART > 3)
    if (uart == 3) {
        uint8_t hs = 0;
        U4BRG = uart_calculate_baud(baud, &hs);
        if (hs) {
            U4MODESET = _U4MODE_BRGH_MASK;
        } else {
            U4MODECLR = _U4MODE_BRGH_MASK;
        }
        return 1;
    }
#endif
#if (__CHIP_HAS_UART > 4)
    if (uart == 4) {
        uint8_t hs = 0;
        U5BRG = uart_calculate_baud(baud, &hs);
        if (hs) {
            U5MODESET = _U5MODE_BRGH_MASK;
        } else {
            U5MODECLR = _U5MODE_BRGH_MASK;
        }
        return 1;
    }
#endif
#if (__CHIP_HAS_UART > 5)
    if (uart == 5) {
        uint8_t hs = 0;
        U6BRG = uart_calculate_baud(baud, &hs);
        if (hs) {
            U6MODESET = _U6MODE_BRGH_MASK;
        } else {
            U6MODECLR = _U6MODE_BRGH_MASK;
        }
        return 1;
    }
#endif

}

/**
 * Configure the data format for the selected UART
 * Formats are specified in convenient macros:
 *   * uart8N1
 *   * uart8N2
 *   * uart8E1
 *   * uart8E2
 *   * uart801
 *   * uart8O2
 *   * uart9N1
 *   * uart9N2
 * @param uart The index of the UART
 * @param format The format to use.
 * @returns 1 on success, 0 on failure
 */
int uart_set_format(uint8_t uart, uint8_t format) {
    if (uart >= __CHIP_HAS_UART) return 0;
#if (__CHIP_HAS_UART > 0)
    if (uart == 0) {
        U1MODECLR = 0b111;
        U1MODESET = (format & 0b111);
        return 1;
    }
#endif

#if (__CHIP_HAS_UART > 1)
    if (uart == 1) {
        U2MODECLR = 0b111;
        U2MODESET = (format & 0b111);
        return 1;
    }
#endif

#if (__CHIP_HAS_UART > 2)
    if (uart == 2) {
        U3MODECLR = 0b111;
        U3MODESET = (format & 0b111);
        return 1;
    }
#endif

#if (__CHIP_HAS_UART > 3)
    if (uart == 3) {
        U4MODECLR = 0b111;
        U4MODESET = (format & 0b111);
        return 1;
    }
#endif

#if (__CHIP_HAS_UART > 4)
    if (uart == 4) {
        U5MODECLR = 0b111;
        U5MODESET = (format & 0b111);
        return 1;
    }
#endif

#if (__CHIP_HAS_UART > 5)
    if (uart == 5) {
        U6MODECLR = 0b111;
        U6MODESET = (format & 0b111);
        return 1;
    }
#endif

    return 0;
}

static void uart_control_task(void *params) {
    uint8_t *paramsu8 = (uint8_t *)params;
    uint8_t uart = *paramsu8;

    while (1) {
        xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
        if (xStreamBufferBytesAvailable(uartControlData[uart].txBuffer) > 0) {
            cpu_set_interrupt_enable(uartControlData[uart].txVector);
            cpu_set_interrupt_flag(uartControlData[uart].txVector);
        }
    }
}

int uart_open(uint8_t uart) {
    if (uart >= __CHIP_HAS_UART) return 0;
    if (uartControlData[uart].task != NULL) {
        return 0;
    }

    uartControlData[uart].txBuffer = xStreamBufferCreate(64, 0);
    uartControlData[uart].rxBuffer = xStreamBufferCreate(64, 0);

    if (xTaskCreate(uart_control_task, uartControlData[uart].name, configMINIMAL_STACK_SIZE, (void *)&uart, tskIDLE_PRIORITY, &uartControlData[uart].task) == pdPASS) {
        uartControlData[uart].task = NULL;
        return 0;
    }

    cpu_set_interrupt_priority(uartControlData[uart].txVector, 2, 0);
    cpu_set_interrupt_priority(uartControlData[uart].rxVector, 2, 0);
//    cpu_set_interrupt_priority(uartControlData[uart].faultVector, 2, 0);
    cpu_clear_interrupt_flag(uartControlData[uart].txVector);
    cpu_clear_interrupt_flag(uartControlData[uart].rxVector);
//    cpu_clear_interrupt_flag(uartControlData[uart].faultVector);
    cpu_set_interrupt_enable(uartControlData[uart].rxVector);
    return 1;
}

int uart_close(uint8_t uart) {
    if (uart >= __CHIP_HAS_UART) return 0;
    if (uartControlData[uart].task == NULL) return 0;
    cpu_clear_interrupt_enable(uartControlData[uart].txVector);
    cpu_clear_interrupt_enable(uartControlData[uart].rxVector);
    cpu_clear_interrupt_enable(uartControlData[uart].faultVector);
    cpu_set_interrupt_priority(uartControlData[uart].txVector, 0, 0);
    cpu_set_interrupt_priority(uartControlData[uart].rxVector, 0, 0);
//    cpu_set_interrupt_priority(uartControlData[uart].faultVector, 0, 0);
    vTaskDelete(uartControlData[uart].task);
    vStreamBufferDelete(uartControlData[uart].txBuffer);
    vStreamBufferDelete(uartControlData[uart].rxBuffer);
    uartControlData[uart].task = NULL;
    return 1;
}

#if (__CHIP_HAS_UART > 0)
void __ISR(_UART1_RX_VECTOR, IPL2) uart_0_rx() {
    cpu_clear_interrupt_flag(_UART1_RX_VECTOR);
    if (uartControlData[0].task == NULL) return;
    uint8_t data = U1RXREG;
    if (xStreamBufferSpacesAvailable(uartControlData[0].rxBuffer) > 0) {
        xStreamBufferSendFromISR(uartControlData[0].rxBuffer, (const void *)&data, 1, NULL);
    }
}
#endif

#if (__CHIP_HAS_UART > 1)
void __ISR(_UART2_RX_VECTOR, IPL2) uart_1_rx() {
    cpu_clear_interrupt_flag(_UART2_RX_VECTOR);
    if (uartControlData[1].task == NULL) return;
    uint8_t data = U2RXREG;
    if (xStreamBufferSpacesAvailable(uartControlData[1].rxBuffer) > 0) {
        xStreamBufferSendFromISR(uartControlData[1].rxBuffer, (const void *)&data, 1, NULL);
    }
}
#endif

#if (__CHIP_HAS_UART > 2)
void __ISR(_UART3_RX_VECTOR, IPL2) uart_2_rx() {
    cpu_clear_interrupt_flag(_UART3_RX_VECTOR);
    if (uartControlData[2].task == NULL) return;
    uint8_t data = U3RXREG;
    if (xStreamBufferSpacesAvailable(uartControlData[2].rxBuffer) > 0) {
        xStreamBufferSendFromISR(uartControlData[2].rxBuffer, (const void *)&data, 1, NULL);
    }
}
#endif

#if (__CHIP_HAS_UART > 3)
void __ISR(_UART4_RX_VECTOR, IPL2) uart_3_rx() {
    cpu_clear_interrupt_flag(_UART4_RX_VECTOR);
    if (uartControlData[3].task == NULL) return;
    uint8_t data = U4RXREG;
    if (xStreamBufferSpacesAvailable(uartControlData[3].rxBuffer) > 0) {
        xStreamBufferSendFromISR(uartControlData[3].rxBuffer, (const void *)&data, 1, NULL);
    }
}
#endif

#if (__CHIP_HAS_UART > 4)
void __ISR(_UART5_RX_VECTOR, IPL2) uart_4_rx() {
    cpu_clear_interrupt_flag(_UART5_RX_VECTOR);
    if (uartControlData[4].task == NULL) return;
    uint8_t data = U5RXREG;
    if (xStreamBufferSpacesAvailable(uartControlData[4].rxBuffer) > 0) {
        xStreamBufferSendFromISR(uartControlData[4].rxBuffer, (const void *)&data, 1, NULL);
    }
}
#endif

#if (__CHIP_HAS_UART > 5)
void __ISR(_UART6_RX_VECTOR, IPL2) uart_5_rx() {
    cpu_clear_interrupt_flag(_UART6_RX_VECTOR);
    if (uartControlData[5].task == NULL) return;
    uint8_t data = U6RXREG;
    if (xStreamBufferSpacesAvailable(uartControlData[5].rxBuffer) > 0) {
        xStreamBufferSendFromISR(uartControlData[5].rxBuffer, (const void *)&data, 1, NULL);
    }
}
#endif




#if (__CHIP_HAS_UART > 0)
void __ISR(_UART1_TX_VECTOR, IPL2) uart_0_tx() {
    cpu_clear_interrupt_flag(_UART1_TX_VECTOR);
    if (uartControlData[0].task == NULL) {
        cpu_clear_interrupt_enable(_UART1_TX_VECTOR);
        return;
    }

    if (xStreamBufferBytesAvailable(uartControlData[0].txBuffer) == 0) {
        cpu_clear_interrupt_enable(_UART1_TX_VECTOR);
        return;
    }

    uint8_t b = 0;

    xStreamBufferReceiveFromISR(uartControlData[0].txBuffer, &b, 1, NULL);
    U1TXREG = b;
}
#endif

#if (__CHIP_HAS_UART > 1)
void __ISR(_UART2_TX_VECTOR, IPL2) uart_1_tx() {
    cpu_clear_interrupt_flag(_UART2_TX_VECTOR);
    if (uartControlData[1].task == NULL) {
        cpu_clear_interrupt_enable(_UART2_TX_VECTOR);
        return;
    }

    if (xStreamBufferBytesAvailable(uartControlData[1].txBuffer) == 0) {
        cpu_clear_interrupt_enable(_UART2_TX_VECTOR);
        return;
    }

    uint8_t b = 0;

    xStreamBufferReceiveFromISR(uartControlData[1].txBuffer, &b, 1, NULL);
    U2TXREG = b;
}
#endif

#if (__CHIP_HAS_UART > 2)
void __ISR(_UART3_TX_VECTOR, IPL2) uart_2_tx() {
    cpu_clear_interrupt_flag(_UART3_TX_VECTOR);
    if (uartControlData[2].task == NULL) {
        cpu_clear_interrupt_enable(_UART3_TX_VECTOR);
        return;
    }

    if (xStreamBufferBytesAvailable(uartControlData[2].txBuffer) == 0) {
        cpu_clear_interrupt_enable(_UART3_TX_VECTOR);
        return;
    }

    uint8_t b = 0;

    xStreamBufferReceiveFromISR(uartControlData[2].txBuffer, &b, 1, NULL);
    U3TXREG = b;
}
#endif

#if (__CHIP_HAS_UART > 3)
void __ISR(_UART4_TX_VECTOR, IPL2) uart_3_tx() {
    cpu_clear_interrupt_flag(_UART4_TX_VECTOR);
    if (uartControlData[3].task == NULL) {
        cpu_clear_interrupt_enable(_UART4_TX_VECTOR);
        return;
    }

    if (xStreamBufferBytesAvailable(uartControlData[3].txBuffer) == 0) {
        cpu_clear_interrupt_enable(_UART4_TX_VECTOR);
        return;
    }

    uint8_t b = 0;

    xStreamBufferReceiveFromISR(uartControlData[3].txBuffer, &b, 1, NULL);
    U4TXREG = b;
}
#endif

#if (__CHIP_HAS_UART > 4)
void __ISR(_UART5_TX_VECTOR, IPL2) uart_4_tx() {
    cpu_clear_interrupt_flag(_UART5_TX_VECTOR);
    if (uartControlData[4].task == NULL) {
        cpu_clear_interrupt_enable(_UART5_TX_VECTOR);
        return;
    }

    if (xStreamBufferBytesAvailable(uartControlData[4].txBuffer) == 0) {
        cpu_clear_interrupt_enable(_UART5_TX_VECTOR);
        return;
    }

    uint8_t b = 0;

    xStreamBufferReceiveFromISR(uartControlData[4].txBuffer, &b, 1, NULL);
    U5TXREG = b;
}
#endif

#if (__CHIP_HAS_UART > 5)
void __ISR(_UART6_TX_VECTOR, IPL2) uart_5_tx() {
    cpu_clear_interrupt_flag(_UART6_TX_VECTOR);
    if (uartControlData[5].task == NULL) {
        cpu_clear_interrupt_enable(_UART6_TX_VECTOR);
        return;
    }

    if (xStreamBufferBytesAvailable(uartControlData[5].txBuffer) == 0) {
        cpu_clear_interrupt_enable(_UART6_TX_VECTOR);
        return;
    }

    uint8_t b = 0;

    xStreamBufferReceiveFromISR(uartControlData[5].txBuffer, &b, 1, NULL);
    U6TXREG = b;
}
#endif

