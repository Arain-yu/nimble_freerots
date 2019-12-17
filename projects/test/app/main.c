/**
  *********************************************************************************
  * @file    main.c
  * @brief   main
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
#include "nimble/ble.h"
#include "host/ble_hs.h"
#include "nimble/nimble_port.h"
#include "ble_appearance_id.h"

/* Hold the address type */
static uint8_t ble_addr_type;

/* Hold the device name */
static const char user_ble_device_name[] = "yuzrain_test";

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
    switch (event->type) {
    case BLE_GAP_EVENT_ADV_COMPLETE:
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
    struct ble_hs_adv_fields fields;

    /*
     *  Set the advertisement data included in our advertisements:
     *     o Flags
     *     o Advertising tx power
     *     o Device name
     *     o Appearance
     */
    memset(&fields, 0, sizeof(fields));

    /*
     * Advertise two flags:
     *      o Discoverability in forthcoming advertisement (general)
     *      o BLE-only (BR/EDR unsupported)
     */
    fields.flags = BLE_HS_ADV_F_BREDR_UNSUP;
    
    /*
     * Indicate that the TX power level field should be included; have the
     * stack fill this value automatically.  This is done by assigning the
     * special value BLE_HS_ADV_TX_PWR_LVL_AUTO.
     */
    fields.tx_pwr_lvl_is_present = 1;
    fields.tx_pwr_lvl = BLE_HS_ADV_TX_PWR_LVL_AUTO;

    fields.name = (uint8_t *)user_ble_device_name;
    fields.name_len = strlen(user_ble_device_name);
    fields.name_is_complete = 1;
    
    fields.appearance_is_present = 1;
    fields.appearance           = BLE_APPEARANCE_GENERIC_PHONE;

    ble_gap_adv_set_fields(&fields);

    /* Begin advertising */
    memset(&adv_params, 0, sizeof(adv_params));
    adv_params.conn_mode = BLE_GAP_CONN_MODE_NON;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_NON;
    ble_gap_adv_start(ble_addr_type, NULL, BLE_HS_FOREVER,
                           &adv_params, user_ble_gap_event, NULL);
}

/**
 * @brief BLE host configuration synchronous callback.
 * @param None
 * @param None
 */
static void user_ble_on_sync(void)
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
    /* Set the host synchronous callback */
    /* This callback is executed when the host and controller become synced. */
    ble_hs_cfg.sync_cb  = user_ble_on_sync;

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

    /* Start FreeRTOS scheduler. */
    vTaskStartScheduler();

    while(1)
    {}
}
