/**
 * @file uart.c
 * Controls the USART peripherals of the PIC32
 */
#include <stdio.h>
#include <sys/attribs.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "sdk/gpio.h"
#include "sdk/uart.h"
#include "sdk/chipspec.h"
#include "sdk/cpu.h"

typedef struct {
    volatile p32_regset mode;
    volatile p32_regset sta;
    volatile p32_regbuf txreg;
    volatile p32_regbuf rxreg;
    volatile p32_regset brg;
} p32_uart;

struct uartControlDataStruct {
#if (configUART_TX_BUFFERED == 1)
#define QUEUES NULL, NULL
    QueueHandle_t txBuffer;
#else
#define QUEUES NULL
#endif
    QueueHandle_t rxBuffer;
    const char *name;
    uint8_t txVector;
    uint8_t rxVector;
    uint8_t faultVector;
    p32_uart *reg;
    SemaphoreHandle_t writeSemaphore;
};

static struct uartControlDataStruct uartControlData[__CHIP_HAS_UART] = {
#if (__CHIP_HAS_UART > 0)
    { QUEUES, "UART0", _UART1_TX_VECTOR, _UART1_RX_VECTOR, _UART1_FAULT_VECTOR, (p32_uart *)&U1MODE, NULL },
#endif
#if (__CHIP_HAS_UART > 1)
    { QUEUES, "UART1", _UART2_TX_VECTOR, _UART2_RX_VECTOR, _UART2_FAULT_VECTOR, (p32_uart *)&U2MODE, NULL },
#endif
#if (__CHIP_HAS_UART > 2)
    { QUEUES, "UART2", _UART3_TX_VECTOR, _UART3_RX_VECTOR, _UART3_FAULT_VECTOR, (p32_uart *)&U3MODE, NULL },
#endif
#if (__CHIP_HAS_UART > 3)
    { QUEUES, "UART3", _UART4_TX_VECTOR, _UART4_RX_VECTOR, _UART4_FAULT_VECTOR, (p32_uart *)&U4MODE, NULL },
#endif
#if (__CHIP_HAS_UART > 4)
    { QUEUES, "UART4", _UART5_TX_VECTOR, _UART5_RX_VECTOR, _UART5_FAULT_VECTOR, (p32_uart *)&U5MODE, NULL },
#endif
#if (__CHIP_HAS_UART > 5)
    { QUEUES, "UART5", _UART6_TX_VECTOR, _UART6_RX_VECTOR, _UART6_FAULT_VECTOR, (p32_uart *)&U6MODE, NULL },
#endif
};

/**
 * Purge all received data from the receive queue of a UART
 * @param uart The UART number (0-5) to purge
 */
void uart_purge(uint8_t uart) {
    xQueueReset(uartControlData[uart].rxBuffer);
}

/**
 * Block until all data is sent out of a UART transmission queue
 * @param uart The UART number (0-5) to flush
 */
#if (configUART_TX_BUFFERED == 1)
void uart_flush(uint8_t uart) {
    if (uartControlData[uart].txBuffer == NULL) return 0;
    while (uxQueueMessagesWaiting(uartControlData[uart].txBuffer) > 0) {
        vTaskDelay(1);
    }
    while ((uartControlData[uart].reg->sta.reg & (1 << 8)) != 0) {
        vTaskDelay(1);
    }
}
#else
void uart_flush(uint8_t uart) {
    while ((uartControlData[uart].reg->sta.reg & (1 << 8)) != 0) {
        vTaskDelay(1);
    }
}
#endif

/**
 * Return the number of bytes available to read in the UART receive buffer
 * @param uart The UART (0-5) to query
 * @returns The number of bytes waiting to be read
 */
int uart_rx_available(uint8_t uart) {
    if (uartControlData[uart].rxBuffer == NULL) return 0;
    int r = uxQueueMessagesWaiting(uartControlData[uart].rxBuffer);
    return r;
}

/**
 * Return the number of spaces left in the transmission buffer of a UART
 * @param uart The UART (0-5) to query
 * @returns The number of bytes space in the transmit queue
 */
#if (configUART_TX_BUFFERED == 1)
int uart_tx_available(uint8_t uart) {
    if (uartControlData[uart].txBuffer == NULL) return 0;
    return uxQueueSpacesAvailable(uartControlData[uart].txBuffer);
}
#else
int uart_tx_available(uint8_t uart) {
    if ((uartControlData[uart].reg->sta.reg & (1 << 9)) == 0) {
        return 1;
    }
    return 0;
}
#endif

/**
 * Write a block of bytes out through the UART
 * @param uart The UART (0-5) to write to
 * @param bytes Pointer to the data to write
 * @param len Length of the data to write
 * @returns The number of bytes able to be written
 */
#if (configUART_TX_BUFFERED == 1)
int uart_write_bytes(uint8_t uart, const uint8_t *bytes, size_t len) {
    if (uartControlData[uart].txBuffer == NULL) return 0;
    int count = 0;
    for (int i = 0; i < len; i++) {
        if (xQueueSendToBack(uartControlData[uart].txBuffer, bytes[i], portMAX_DELAY) == pdPASS) {
            if (!cpu_get_interrupt_enable(uartControlData[uart].txVector)) {
                cpu_set_interrupt_flag(uartControlData[uart].txVector);
                cpu_set_interrupt_enable(uartControlData[uart].txVector);
            }
            // For some reason this is needed to allow the queue to
            // synchronise with the ISR properly. Any clues?
            vTaskDelay(1);
        } else {
            break;
        }
        count++;
    }
    xSemaphoreGive(uartControlData[uart].writeSemaphore);
    return count;
}
#else
int uart_write_bytes(uint8_t uart, const uint8_t *bytes, size_t len) {
    if (xSemaphoreTake(uartControlData[uart].writeSemaphore, portMAX_DELAY) != pdPASS) {
        return 0;
    }
    int count = 0;
    for (int i = 0; i < len; i++) {
        while ((uartControlData[uart].reg->sta.reg & (1 << 9)) != 0) {
            vTaskDelay(1);
        }

        uartControlData[uart].reg->txreg.reg = bytes[i];
        count++;
    }
    xSemaphoreGive(uartControlData[uart].writeSemaphore);
    return count;
}
#endif

/**
 * Write an array of bytes to a UART in an emergency
 * situation. This bypasses any buffering (if enabled)
 * and the thread safety mutex. For use in exception
 * handlers and the like where panic information needs
 * to be reported before shutting down or rebooting
 * @param uart The UART number
 * @param bytes The array of bytes to print
 * @param len The number of bytes to write
 * @returns The number of bytes written
 */
int uart_write_bytes_emergency(uint8_t uart, const uint8_t *bytes, size_t len) {
    int count = 0;
    for (int i = 0; i < len; i++) {
        while ((uartControlData[uart].reg->sta.reg & (1 << 9)) != 0) {
            continue;
        }

        uartControlData[uart].reg->txreg.reg = bytes[i];
        count++;
    }
    while ((uartControlData[uart].reg->sta.reg & (1 << 8)) != 0) {
        continue;
    }
    return count;
}

/**
 * Write a single byte through the UART
 * @param uart The UART (0-5) to write to
 * @param byte The data to write
 * @returns 1 if the data could be written, 0 otherwise
 */
#if (configUART_TX_BUFFERED == 1)
int uart_write(uint8_t uart, uart_queue_t byte) {
    if (uartControlData[uart].txBuffer == NULL) return 0;
    if (xSemaphoreTake(uartControlData[uart].writeSemaphore, portMAX_DELAY) != pdPASS) {
        return 0;
    }
    if (xQueueSendToBack(uartControlData[uart].txBuffer, &byte, portMAX_DELAY) == pdPASS) {
        if (!cpu_get_interrupt_enable(uartControlData[uart].txVector)) {
            cpu_set_interrupt_flag(uartControlData[uart].txVector);
            cpu_set_interrupt_enable(uartControlData[uart].txVector);
            // For some reason this is needed to allow the queue to
            // synchronise with the ISR properly. Any clues?
            vTaskDelay(1);
        }
        xSemaphoreGive(uartControlData[uart].writeSemaphore);
        return 1;
    }
    return 0;
}
#else
int uart_write(uint8_t uart, uart_queue_t byte) {
    if (xSemaphoreTake(uartControlData[uart].writeSemaphore, portMAX_DELAY) != pdPASS) {
        return 0;
    }
    while ((uartControlData[uart].reg->sta.reg & (1 << 9)) != 0) {
        vTaskDelay(1);
    }

    uartControlData[uart].reg->txreg.reg = byte;
    xSemaphoreGive(uartControlData[uart].writeSemaphore);
    return 1;
}
#endif


/**
 * Read a single byte from the UART receive queue
 * @param uart The number of the UART (0-5) to read from
 * @returns The next byte in the queue, or -1 if the queue is empty
 */
int uart_read(uint8_t uart) {
    if (uxQueueMessagesWaiting(uartControlData[uart].rxBuffer) == 0) {
        return -1;
    }
    uart_queue_t b = 0;
    xQueueReceive(uartControlData[uart].rxBuffer, &b, 0);
    return b;
}

/**
 * Read the next byte from the UART receive queue without removing it from the queue
 * param uart The number of the UART (0-5) to read from
 * @returns The next byte in the queue, or -1 if the queue is empty
 */
int uart_peek(uint8_t uart) {
    if (uxQueueMessagesWaiting(uartControlData[uart].rxBuffer) == 0) {
        return -1;
    }
    uart_queue_t b = 0;
    xQueuePeek(uartControlData[uart].rxBuffer, &b, 0);
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
    switch (uart) {
        case 0: return gpio_set_output_function(pin, gpioPPS_U1TX);
        case 1: return gpio_set_output_function(pin, gpioPPS_U2TX);
        case 2: return gpio_set_output_function(pin, gpioPPS_U3TX);
        case 3: return gpio_set_output_function(pin, gpioPPS_U4TX);
        case 4: return gpio_set_output_function(pin, gpioPPS_U5TX);
        case 5: return gpio_set_output_function(pin, gpioPPS_U6TX);
    }
    return 0;
}

/** 
 * Configure the RX pin of the selected UART through PPS
 * @param uart The index of the UART
 * @param pin The index of the pin to assign the RX function to
 * @returns 1 on success, 0 on failure
 */
int uart_set_rx_pin(uint8_t uart, uint8_t pin) {
    if (uart >= __CHIP_HAS_UART) return 0;
    switch (uart) {
        case 0: return gpio_set_input_function(pin, gpioPPS_U1RX);
        case 1: return gpio_set_input_function(pin, gpioPPS_U2RX);
        case 2: return gpio_set_input_function(pin, gpioPPS_U3RX);
        case 3: return gpio_set_input_function(pin, gpioPPS_U4RX);
        case 4: return gpio_set_input_function(pin, gpioPPS_U5RX);
        case 5: return gpio_set_input_function(pin, gpioPPS_U6RX);
    }
    return 0;
}

static inline uint32_t uart_calculate_baud(uint32_t baud, uint8_t *highspeed) {
    uint32_t brg;
    if (baud < 200000) {
        brg = ((cpu_get_peripheral_clock() / 16 / baud) - 1);
        *highspeed = 1;
    } else {
        brg = ((cpu_get_peripheral_clock() / 4 / baud) - 1);
        *highspeed = 0;
    }
    return brg;
}

/**
 * Configure the baud rate of the selected UART
 * @param uart The index of the UART
 * @param baud The baud rate to configure.
 * @returns 1 on success, 0 on failure
 */
int uart_set_baud(uint8_t uart, uint32_t baud) {
    if (uart >= __CHIP_HAS_UART) return 0;
    uint8_t hs = 0;
    uartControlData[uart].reg->brg.reg = uart_calculate_baud(baud, &hs);
    if (hs) {
        uartControlData[uart].reg->mode.set = _U1MODE_BRGH_MASK;
    } else {
        uartControlData[uart].reg->mode.clr = _U1MODE_BRGH_MASK;
    }
    return 1;
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
    uartControlData[uart].reg->mode.clr = 0b111;
    uartControlData[uart].reg->mode.set = (format & 0b111);
    return 1;
}

/**
 * Open a UART. This configures the UART and starts the transmit and receive queues and
 * interrupts.
 * @param uart The UART index (0-5) to open
 * @returns 1 if the UART could be opened, 0 otherwise 
 */
int uart_open(uint8_t uart) {
    if (uart >= __CHIP_HAS_UART) return 0;

#if (configUART_TX_BUFFERED == 1)
    uartControlData[uart].txBuffer = xQueueCreate(64, sizeof(uart_queue_t));
    cpu_set_interrupt_priority(uartControlData[uart].txVector, 2, 0);
    cpu_clear_interrupt_flag(uartControlData[uart].txVector);
#endif

    uartControlData[uart].rxBuffer = xQueueCreate(64, sizeof(uart_queue_t));
    uartControlData[uart].writeSemaphore = xSemaphoreCreateMutex();

    cpu_set_interrupt_priority(uartControlData[uart].rxVector, 2, 0);
    cpu_clear_interrupt_flag(uartControlData[uart].rxVector);
    cpu_set_interrupt_enable(uartControlData[uart].rxVector);

//    cpu_set_interrupt_priority(uartControlData[uart].faultVector, 2, 0);
//    cpu_clear_interrupt_flag(uartControlData[uart].faultVector);

    uartControlData[uart].reg->sta.set = (1 << 12) | (1 << 10);
    uartControlData[uart].reg->mode.set = (1 << 15);

    return 1;
}

/**
 * Test if a UART has been opened
 * @param uart The UART to test
 * @returns 1 if the UART is open, 0 otherwise
 */
int uart_is_open(uint8_t uart) {
    if (uartControlData[uart].rxBuffer == NULL) return 0;
    if (cpu_get_interrupt_enable(uartControlData[uart].rxVector) == 0) return 0;
    if ((uartControlData[uart].reg->mode.reg & (1 << 15)) == 0) return 0;
    return 1;
}

/**
 * Close a UART and free any resources used
 * @param uart The UART index (0-5) to close
 * @returns 1 if the UART could be closed, 0 otherwise
 */
int uart_close(uint8_t uart) {
    if (uart >= __CHIP_HAS_UART) return 0;
    uartControlData[uart].reg->sta.clr = (1 << 12) | (1 << 10);
    uartControlData[uart].reg->mode.clr = (1 << 15);
#if (configUART_TX_BUFFERED == 1)
    cpu_clear_interrupt_enable(uartControlData[uart].txVector);
    cpu_set_interrupt_priority(uartControlData[uart].txVector, 0, 0);
    vQueueDelete(uartControlData[uart].txBuffer);
#endif

    cpu_clear_interrupt_enable(uartControlData[uart].rxVector);
    cpu_set_interrupt_priority(uartControlData[uart].rxVector, 0, 0);
    vQueueDelete(uartControlData[uart].rxBuffer);
    vSemaphoreDelete(uartControlData[uart].writeSemaphore);

    cpu_clear_interrupt_enable(uartControlData[uart].faultVector);
    cpu_set_interrupt_priority(uartControlData[uart].faultVector, 0, 0);
    return 1;
}

static void inline uart_handle_rx(uint8_t uart) {
    cpu_clear_interrupt_flag(uartControlData[uart].rxVector);
    if (uartControlData[uart].reg->sta.reg & 1) {
        uart_queue_t data = uartControlData[uart].reg->rxreg.reg;
        if (uxQueueSpacesAvailable(uartControlData[uart].rxBuffer) > 0) {
            xQueueSendToBackFromISR(uartControlData[uart].rxBuffer, &data, NULL);
        }
    }
}

#if (__CHIP_HAS_UART > 0)
void __ISR(_UART1_RX_VECTOR, IPL2AUTO) uart_0_rx() { uart_handle_rx(0); }
#endif

#if (__CHIP_HAS_UART > 1)
void __ISR(_UART2_RX_VECTOR, IPL2AUTO) uart_1_rx() { uart_handle_rx(1); }
#endif

#if (__CHIP_HAS_UART > 2)
void __ISR(_UART3_RX_VECTOR, IPL2AUTO) uart_2_rx() { uart_handle_rx(2); }
#endif

#if (__CHIP_HAS_UART > 3)
void __ISR(_UART4_RX_VECTOR, IPL2AUTO) uart_3_rx() { uart_handle_rx(3); }
#endif

#if (__CHIP_HAS_UART > 4)
void __ISR(_UART5_RX_VECTOR, IPL2AUTO) uart_4_rx() { uart_handle_rx(4); }
#endif

#if (__CHIP_HAS_UART > 5)
void __ISR(_UART6_RX_VECTOR, IPL2AUTO) uart_5_rx() { uart_handle_rx(5); }
#endif



#if (configUART_TX_BUFFERED == 1)
static inline void uart_handle_tx(uint8_t uart) {
    cpu_clear_interrupt_flag(uartControlData[uart].txVector);

    if (uxQueueMessagesWaitingFromISR(uartControlData[uart].txBuffer) == 0) {
        cpu_clear_interrupt_enable(uartControlData[uart].txVector);
        return;
    }

    uart_queue_t b = 0;
    xQueueReceiveFromISR(uartControlData[uart].txBuffer, &b, NULL);
    uartControlData[uart].reg->txreg.reg = b;
}


#if (__CHIP_HAS_UART > 0)
void __ISR(_UART1_TX_VECTOR, IPL2AUTO) uart_0_tx() { uart_handle_tx(0); }
#endif

#if (__CHIP_HAS_UART > 1)
void __ISR(_UART2_TX_VECTOR, IPL2AUTO) uart_1_tx() { uart_handle_tx(1); }
#endif

#if (__CHIP_HAS_UART > 2)
void __ISR(_UART3_TX_VECTOR, IPL2AUTO) uart_2_tx() { uart_handle_tx(2); }
#endif

#if (__CHIP_HAS_UART > 3)
void __ISR(_UART4_TX_VECTOR, IPL2AUTO) uart_3_tx() { uart_handle_tx(3); }
#endif

#if (__CHIP_HAS_UART > 4)
void __ISR(_UART5_TX_VECTOR, IPL2AUTO) uart_4_tx() { uart_handle_tx(4); }
#endif

#if (__CHIP_HAS_UART > 5)
void __ISR(_UART6_TX_VECTOR, IPL2AUTO) uart_5_tx() { uart_handle_tx(5); }
#endif
#endif

