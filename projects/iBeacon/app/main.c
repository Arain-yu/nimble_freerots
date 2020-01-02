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

#define IBEACON_MFG_DATA_SIZE       25

/* Hold the address type */
static uint8_t ble_addr_type;

/* Company ID: Apple */
uint16_t user_ble_company_id = 0x004C;
/* iBeacon indicator */
uint16_t user_ble_ibeacon_indicator = 0x1502;
/* WeChat iBeacon UUID */
const char user_ble_weixin_uuid[] = {0xFD, 0xA5, 0x06, 0x93,
                                     0xA4, 0xE2, 0x4F, 0xB1,
                                     0xAF, 0xCF, 0xC6, 0XEB,
                                     0x07, 0x64, 0x78, 0x25};
/* WeChat merchant service number */
uint16_t user_ble_weixin_major = 10002;
/* WeChat merchant service device number */
uint16_t user_ble_weixin_minor = 12345;
/* Power */
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
    struct ble_hs_adv_fields fields;
    uint8_t buf[IBEACON_MFG_DATA_SIZE];

    memset(&fields, 0, sizeof (fields));
    memset(&buf, 0, IBEACON_MFG_DATA_SIZE);
    memset(&adv_params, 0, sizeof(adv_params));

    /* Test */
//    ble_ibeacon_set_adv_data((void *)(&user_ble_weixin_uuid), user_ble_weixin_major,
//                        user_ble_weixin_minor, user_ble_weixin_power);

    /* The iBeacon data is devided into 4 parts:
     * @1 iBeacon prefix (fixed)
     * @2 proximity UUID
     * @3 major
     * @4 minor
     * @5 s complement of measured TX power
     * The iBeacon prefix section contains FLAG data from the
     * general broadcast data as well as producer specific data.
     * FLAG [0x02(length) 0x01(type) 0x06(data)]
     * mfg_data [0x1A(length) 0xFF(type) ......(data)]
     */

    /* Advertise two flags:
     *     o Discoverability in forthcoming advertisement (general)
     *     o BLE-only (BR/EDR unsupported).
     */
    fields.flags = BLE_HS_ADV_F_DISC_GEN |
                   BLE_HS_ADV_F_BREDR_UNSUP;

    /* The following is manufacture data
     * @1 Company ID
     * @2 iBeacon indicator
     * @3 WeChat iBeacon UUID
     * @4 major
     * @5 minor
     * @6 Power
     */
    memcpy(&buf[0], &user_ble_company_id, 2);
    memcpy(&buf[0+2], &user_ble_ibeacon_indicator, 2);
    memcpy(&buf[0+2+2], user_ble_weixin_uuid, 16);
    put_be16(buf + 20, user_ble_weixin_major);
    put_be16(buf + 22, user_ble_weixin_minor);
    memcpy(&buf[0+2+2+16+2+2], &user_ble_weixin_power, 1);
    fields.mfg_data     = buf;
    fields.mfg_data_len = sizeof (buf);
    ble_gap_adv_set_fields(&fields);

    /* Begin advertising */
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


