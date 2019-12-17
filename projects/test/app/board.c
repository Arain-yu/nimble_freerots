/**
  *********************************************************************************
  * @file    board.c
  * @brief   PCA10040 board configuration
  * @version V1.0
  * @author  Yuzrain
  *********************************************************************************
  */

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "nordic_common.h"
#include "nrf_drv_clock.h"
#include "nrf_gpio.h"
#include "board.h"

/* LED timer handle */
static TimerHandle_t led_toggle_timer;

/**
 * @brief Board LED toggle timer.
 *
 * @param arg Pointer to the LED toggle timer function
 * @param None
 */
static void led_toggle_timer_callback (TimerHandle_t timer)
{
    nrf_gpio_pin_toggle(LED_1);
    nrf_gpio_pin_toggle(LED_2);
    nrf_gpio_pin_toggle(LED_3);
    nrf_gpio_pin_toggle(LED_4);
}

/**
 * @brief Board hardware configuration.
 *
 * @param None
 * @param None
 */
void board_init(void)
{
    /* Clock configuration */
    nrf_drv_clock_init();
    
    /* LED configuration */
    nrf_gpio_cfg_output(LED_1);
    nrf_gpio_cfg_output(LED_2);
    nrf_gpio_cfg_output(LED_3);
    nrf_gpio_cfg_output(LED_4);

    /* Create LED toggle timer function */
    led_toggle_timer = xTimerCreate( "LED1",
                                    100,
                                    pdTRUE,
                                    NULL,
                                    led_toggle_timer_callback);
    xTimerStart(led_toggle_timer, 0);
}
