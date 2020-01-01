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

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "nordic_common.h"
#include "nrf_drv_clock.h"
#include "nrf_gpio.h"
#include "board.h"
#include "nimble/ble.h"
#include "host/ble_hs.h"
#include "nimble/nimble_port.h"
#include "ble_appearance_id.h"

/* Hold the address type */
static uint8_t ble_addr_type;

/* Hold the device name */
const char user_ble_weixin_uuid[] = {0xFD, 0xA5, 0x06, 0x93,
                                     0xA4, 0xE2, 0x4F, 0xB1,
                                     0xAF, 0xCF, 0xC6, 0XEB,
                                     0x07, 0x64, 0x78, 0x25};
uint16_t user_ble_weixin_major = 10002;
uint16_t user_ble_weixin_minor = 12345;
int8_t user_ble_weixin_power = 0xC3;


/* Hold LED status */
extern volatile uint32_t led_status;

/* Functions */
extern void nimble_port_freertos_init(TaskFunction_t host_task_fn);
void user_advertise_init(void);

/**
 * @brief BLE gap event handler.
 * @param event Pointer to the event(event type + event data)
 * @param arg 
 * @param None
 */
static int user_ble_gap_event(struct ble_gap_event *event, void *arg)
{
    switch (event->type) 
    {
        case BLE_GAP_EVENT_ADV_COMPLETE:
            printf("[adv end]\r\n");
            user_advertise_init();

            break;

        default:
            break;
    }

    return 0;
}

/**
 * @brief BLE advertise configuration.
 * @param None
 * @param None
 */
void user_advertise_init(void)
{
    struct ble_gap_adv_params adv_params;
    

    ble_ibeacon_set_adv_data((void *)(&user_ble_weixin_uuid), user_ble_weixin_major,
                        user_ble_weixin_minor, user_ble_weixin_power);

    /* Begin advertising */
    memset(&adv_params, 0, sizeof(adv_params));
    adv_params.conn_mode = BLE_GAP_CONN_MODE_NON;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_NON;
    printf("[adv start] conn_mode:%d|disc_mode:%d\r\n",
                                adv_params.conn_mode,
                                adv_params.disc_mode);
    ble_gap_adv_start(ble_addr_type, NULL, BLE_HS_FOREVER,
                           &adv_params, user_ble_gap_event, NULL);
}

/**
 * @brief BLE host configuration synchronous callback.
 * @param None
 * @param None
 */
static void user_ble_sync_callback(void)
{
    int rc;

    /* Use privacy */
    rc = ble_hs_id_infer_auto(0, &ble_addr_type);
    assert(rc == 0);

    /* Begin advertising */
    user_advertise_init();
}

/**
 * @brief BLE host function entry.
 * @param None
 * @param None
 */
void user_ble_host_entry(void *arg)
{
    /* Debug */
    printf("[Host entry]\r\n");

    /* Set the host synchronous callback */
    /* This callback is executed when the host and controller become synced. */
    ble_hs_cfg.sync_cb              = user_ble_sync_callback;

    nimble_port_run();
}

/**
 * @brief Main.
 * @param None
 * @param None
 */
int main(void)
{
    /* Board init */
    board_init();

    /* Init the BLE stack */
    nimble_port_init();
    nimble_port_freertos_init(user_ble_host_entry);

    printf("[System start]:\r\n");
    /* Start FreeRTOS scheduler. */
    vTaskStartScheduler();

    while(1)
    {}
}


