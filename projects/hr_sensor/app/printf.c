/**
  *********************************************************************************
  * SPDX-License-Identifier: Apache-2.0
  *
  * @file    main.c
  * @brief   main
  * @version V1.0
  * @author  yuzrain
  *
  *********************************************************************************
  */

#include "nrf.h"
#include "nrf_gpio.h"
#include "nrfx_uart.h"
#include "board.h"

/**
 * @brief Remap stdio printf.
 * @param None
 * @param None
 */
int fputc(int ch, FILE *f)
{
    nrf_uart_event_clear(NRF_UART0, NRF_UART_EVENT_TXDRDY);
    nrf_uart_task_trigger(NRF_UART0, NRF_UART_TASK_STARTTX);
    nrf_uart_txd_set(NRF_UART0, (uint8_t)ch);
    while (!nrf_uart_event_check(NRF_UART0, NRF_UART_EVENT_TXDRDY))
    {
    }
    nrf_uart_task_trigger(NRF_UART0, NRF_UART_TASK_STOPTX);
    return 0;
}

/**
 * @brief Config UART.
 * @param None
 * @param None
 */
void printf_init(void)
{
    nrf_uart_disable(NRF_UART0);

    nrf_gpio_pin_set(TX_PIN_NUMBER);
    nrf_gpio_cfg_output(TX_PIN_NUMBER);
    nrf_gpio_pin_clear(TX_PIN_NUMBER);
    nrf_gpio_cfg_input(RX_PIN_NUMBER, GPIO_PIN_CNF_PULL_Pullup);
    nrf_uart_baudrate_set(NRF_UART0, NRF_UART_BAUDRATE_115200);
    nrf_uart_configure(NRF_UART0, NRF_UART_PARITY_EXCLUDED, NRF_UART_HWFC_DISABLED);
    nrf_uart_txrx_pins_set(NRF_UART0, TX_PIN_NUMBER, RX_PIN_NUMBER);

    nrf_uart_enable(NRF_UART0);
}
