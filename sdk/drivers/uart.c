#include <stdio.h>
#include <sys/attribs.h>

#include "FreeRTOS.h"
#include "task.h"
#include "stream_buffer.h"

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

static struct uartControlDataStruct {
    TaskHandle_t task;
    StreamBufferHandle_t txBuffer;
    StreamBufferHandle_t rxBuffer;
    const char *name;
    uint8_t txVector;
    uint8_t rxVector;
    uint8_t faultVector;
    p32_uart *reg;
};

static struct uartControlDataStruct uartControlData[__CHIP_HAS_UART] = {
#if (__CHIP_HAS_UART > 0)
    { NULL, NULL, NULL, "UART0", _UART1_TX_VECTOR, _UART1_RX_VECTOR, _UART1_FAULT_VECTOR, &U1MODE },
#endif
#if (__CHIP_HAS_UART > 1)
    { NULL, NULL, NULL, "UART1", _UART2_TX_VECTOR, _UART2_RX_VECTOR, _UART2_FAULT_VECTOR, &U2MODE },
#endif
#if (__CHIP_HAS_UART > 2)
    { NULL, NULL, NULL, "UART2", _UART3_TX_VECTOR, _UART3_RX_VECTOR, _UART3_FAULT_VECTOR, &U3MODE },
#endif
#if (__CHIP_HAS_UART > 3)
    { NULL, NULL, NULL, "UART3", _UART4_TX_VECTOR, _UART4_RX_VECTOR, _UART4_FAULT_VECTOR, &U4MODE },
#endif
#if (__CHIP_HAS_UART > 4)
    { NULL, NULL, NULL, "UART4", _UART5_TX_VECTOR, _UART5_RX_VECTOR, _UART5_FAULT_VECTOR, &U5MODE },
#endif
#if (__CHIP_HAS_UART > 5)
    { NULL, NULL, NULL, "UART5", _UART6_TX_VECTOR, _UART6_RX_VECTOR, _UART6_FAULT_VECTOR, &U6MODE },
#endif
};

int uart_write(uint8_t uart, uint8_t byte) {
    if (uartControlData[uart].txBuffer == NULL) return 0;
    if (uartControlData[uart].task == NULL) return 0;
    int ret = xStreamBufferSend(uartControlData[uart].txBuffer, &byte, 1, pdMS_TO_TICKS(10000));
    if (ret > 0) {

        xTaskNotify(uartControlData[uart].task, 0, eNoAction);
    }
    return 1; // ret;
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
        highspeed = 1;
    } else {
        brg = ((cpu_get_peripheral_clock() / 4 / baud) - 1);
        highspeed = 1;
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

    if (xTaskCreate(uart_control_task, uartControlData[uart].name, configMINIMAL_STACK_SIZE, (void *)&uart, tskIDLE_PRIORITY, &uartControlData[uart].task) != pdPASS) {
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

    uartControlData[uart].reg->sta.set = (1 << 12) | (1 << 10);
    uartControlData[uart].reg->mode.set = (1 << 15);

    return 1;
}

int uart_close(uint8_t uart) {
    if (uart >= __CHIP_HAS_UART) return 0;
    if (uartControlData[uart].task == NULL) return 0;
    uartControlData[uart].reg->sta.clr = (1 << 12) | (1 << 10);
    uartControlData[uart].reg->mode.clr = (1 << 15);
    cpu_clear_interrupt_enable(uartControlData[uart].txVector);
    cpu_clear_interrupt_enable(uartControlData[uart].rxVector);
    cpu_clear_interrupt_enable(uartControlData[uart].faultVector);
    cpu_set_interrupt_priority(uartControlData[uart].txVector, 0, 0);
    cpu_set_interrupt_priority(uartControlData[uart].rxVector, 0, 0);
    cpu_set_interrupt_priority(uartControlData[uart].faultVector, 0, 0);
    vTaskDelete(uartControlData[uart].task);
    vStreamBufferDelete(uartControlData[uart].txBuffer);
    vStreamBufferDelete(uartControlData[uart].rxBuffer);
    uartControlData[uart].task = NULL;
    return 1;
}

static void inline uart_handle_rx(uint8_t uart) {
    cpu_clear_interrupt_flag(uartControlData[uart].rxVector);
    if (uartControlData[uart].task == NULL) return;
    uint8_t data = uartControlData[uart].reg->rxreg.reg;
    if (xStreamBufferSpacesAvailable(uartControlData[uart].rxBuffer) > 0) {
        xStreamBufferSendFromISR(uartControlData[uart].rxBuffer, (const void *)&data, 1, NULL);
    }
}

#if (__CHIP_HAS_UART > 0)
void __ISR(_UART1_RX_VECTOR, IPL2) uart_0_rx() { uart_handle_rx(0); }
#endif

#if (__CHIP_HAS_UART > 1)
void __ISR(_UART2_RX_VECTOR, IPL2) uart_1_rx() { uart_handle_rx(1); }
#endif

#if (__CHIP_HAS_UART > 2)
void __ISR(_UART3_RX_VECTOR, IPL2) uart_2_rx() { uart_handle_rx(2); }
#endif

#if (__CHIP_HAS_UART > 3)
void __ISR(_UART4_RX_VECTOR, IPL2) uart_3_rx() { uart_handle_rx(3); }
#endif

#if (__CHIP_HAS_UART > 4)
void __ISR(_UART5_RX_VECTOR, IPL2) uart_4_rx() { uart_handle_rx(4); }
#endif

#if (__CHIP_HAS_UART > 5)
void __ISR(_UART6_RX_VECTOR, IPL2) uart_5_rx() { uart_handle_rx(5); }
#endif



static inline void uart_handle_tx(uint8_t uart) {
    cpu_clear_interrupt_flag(uartControlData[uart].txVector);
    if (uartControlData[uart].task == NULL) {
        cpu_clear_interrupt_enable(uartControlData[uart].txVector);
        return;
    }

    if (xStreamBufferBytesAvailable(uartControlData[uart].txBuffer) == 0) {
        cpu_clear_interrupt_enable(uartControlData[uart].txVector);
        return;
    }

    uint8_t b = 0;
    xStreamBufferReceiveFromISR(uartControlData[uart].txBuffer, &b, 1, NULL);
    uartControlData[uart].reg->txreg.reg = b;
}


#if (__CHIP_HAS_UART > 0)
void __ISR(_UART1_TX_VECTOR, IPL2) uart_0_tx() { uart_handle_tx(0); }
#endif

#if (__CHIP_HAS_UART > 1)
void __ISR(_UART2_TX_VECTOR, IPL2) uart_1_tx() { uart_handle_tx(1); }
#endif

#if (__CHIP_HAS_UART > 2)
void __ISR(_UART3_TX_VECTOR, IPL2) uart_2_tx() { uart_handle_tx(2); }
#endif

#if (__CHIP_HAS_UART > 3)
void __ISR(_UART4_TX_VECTOR, IPL2) uart_3_tx() { uart_handle_tx(3); }
#endif

#if (__CHIP_HAS_UART > 4)
void __ISR(_UART5_TX_VECTOR, IPL2) uart_4_tx() { uart_handle_tx(4); }
#endif

#if (__CHIP_HAS_UART > 5)
void __ISR(_UART6_TX_VECTOR, IPL2) uart_5_tx() { uart_handle_tx(5); }
#endif


