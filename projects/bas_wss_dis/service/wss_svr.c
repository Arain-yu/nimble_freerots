/**
  *********************************************************************************
  * SPDX-License-Identifier: Apache-2.0
  *
  * @file    wss_svr.c
  * @brief   Heart rate service definition
  * @version V1.0
  * @author  yuzrain
  *
  *********************************************************************************
  */

#include "FreeRTOS.h"
#include "task.h"
#include "nimble/ble.h"
#include "host/ble_hs.h"
#include "host/ble_uuid.h"
#include "wss_svr.h"
#include "ble_srv_common.h"
#include "nimble/nimble_port.h"

/* Indication timer */
struct ble_npl_callout wss_timer;

/* Hold the indication state */
static bool wss_indication_state;

/* Hold the service handle */
uint16_t wss_service_handle;

/* Hold wss indication handle */
uint16_t wss_indica_handle;

/* Hold the weight scale feature */
static struct{
    uint32_t sclfeat_timstamp_supted:1;
    uint32_t sclfeat_muluser_supted:1;
    uint32_t sclfeat_bmi_supted:1;
    uint32_t sclfeat_weight_res:4;
    uint32_t sclfeat_height_res:3;
    uint32_t reserver:22;
} wss_feature = {
    .sclfeat_timstamp_supted = 0,
    .sclfeat_muluser_supted  = 1,
    .sclfeat_bmi_supted      = 0,
    .sclfeat_weight_res      = 3,/* Resolution of 0.5 kg or 1 lb */
    .sclfeat_height_res      = 0
};

/* Hold the weight value 70Kg */
uint16_t weight_value = 70;

/* Functions */
static int
gatt_svr_chr_access_wss(uint16_t conn_handle, uint16_t attr_handle,
                               struct ble_gatt_access_ctxt *ctxt, void *arg);

/**
 * @brief Weight Scale service database.
 */
static const struct ble_gatt_svc_def wss_svr_svcs[] = {
    {
        /* Service: Heart-rate */
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(BLE_UUID_WEIGHT_SCALE_SERVICE),
        .characteristics = (struct ble_gatt_chr_def[])
        { 
            {
                /* Characteristic: weight_scale_feature UUID */
                .uuid = BLE_UUID16_DECLARE(BLE_UUID_WEIGHT_SCALE_FEATURE_CHAR),
                .access_cb = gatt_svr_chr_access_wss,
                .flags = BLE_GATT_CHR_F_READ,
            },
            {
                /* Characteristic: weight measurement UUID */
                .uuid = BLE_UUID16_DECLARE(BLE_UUID_WEIGHT_MEASURE_CHAR),
                .access_cb = gatt_svr_chr_access_wss,
                .val_handle = &wss_service_handle,
                .flags = BLE_GATT_CHR_F_INDICATE|BLE_GATT_CHR_F_READ,
            },
            {
                0, /* No more characteristics in this service */
            }, 
        },
    },
    {
        .type = BLE_GATT_SVC_TYPE_END, /* No more services */
    },
};

/**
 * @brief Access Weight Scale characteristics.
 * @retval None
 */
static int gatt_svr_chr_access_wss(uint16_t conn_handle, uint16_t attr_handle,
                               struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    uint16_t uuid;
    int rc;
    /* Hold the weight */
    uint8_t weight_data[4];

    uuid = ble_uuid_u16(ctxt->chr->uuid);

    switch (uuid)
    {
        case BLE_UUID_WEIGHT_SCALE_FEATURE_CHAR:
        {
            /* ATT req */
            printf("[att req] read weight scale feature \r\n");
            /* Copy location to send buffer */
            rc = os_mbuf_append(ctxt->om, &wss_feature, 4);

            break;
        }
        case BLE_UUID_WEIGHT_MEASURE_CHAR:
        {
            /* ATT req */
            printf("[att req] read weight value \r\n");
            weight_data[0] = (0x1<<2);  /* User id supported */
            weight_data[1] = (uint8_t)(weight_value*200);
            weight_data[2] = (uint8_t)((weight_value*200)>>8);
            weight_data[3] = 1;       /* User id */
            if (weight_value++ == 80)
            {
                weight_value = 70;
            }
            /* Copy location to send buffer */
            rc = os_mbuf_append(ctxt->om, &weight_data, 4);

            break;
        }

        default:
            break;
    }

    rc = (rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES);

    return rc;
}

/**
 * @brief Heart-rate subscribe handler.
 * @param None
 * @param None
 */
void wss_service_subscribe_handler(struct ble_gap_event *event, void *arg)
{
    if (event->type != BLE_GAP_EVENT_SUBSCRIBE)
        return;

    printf("[wss subscribe event] cur_indication:%d | service handle=%d\r\n",
                    event->subscribe.cur_indicate, event->subscribe.attr_handle);

    if (event->subscribe.attr_handle == wss_service_handle)
    {
        wss_indication_state = event->subscribe.cur_indicate;
        wss_indica_handle = event->subscribe.conn_handle;
        ble_npl_callout_reset(&wss_timer, configTICK_RATE_HZ*5);
    }
}

/**
 * @brief Send heart-Rate noyify.
 * @param None
 * @param None
 */
static void wss_weight_indication(struct ble_npl_event *ev)
{
    int rc;

    if (!wss_indication_state) {
        weight_value = 70;
        ble_npl_callout_stop(&wss_timer);
        return;
    }

    /* Indicate the current weight value */
    rc = ble_gattc_indicate(wss_indica_handle, wss_service_handle);

    assert(rc == 0);
    ble_npl_callout_reset(&wss_timer, configTICK_RATE_HZ*5);
}

/**
 * @brief Weight Scale service configuration.
 * @param None
 * @param None
 */
int wss_svr_init(void)
{
    int rc;

    /* Config weight value indication timer 1S/1-indication */
    ble_npl_callout_init(&wss_timer, nimble_port_get_dflt_eventq(),
                                            wss_weight_indication, NULL);

    rc = ble_gatts_count_cfg(wss_svr_svcs);
    if (rc != 0) {
        return rc;
    }

    rc = ble_gatts_add_svcs(wss_svr_svcs);
    if (rc != 0) {
        return rc;
    }

    return 0;
}
