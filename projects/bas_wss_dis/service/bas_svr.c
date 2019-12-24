/**
  *********************************************************************************
  * SPDX-License-Identifier: Apache-2.0
  *
  * @file    bas_svr.c
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
#include "bas_svr.h"
#include "ble_srv_common.h"
#include "nimble/nimble_port.h"

/* Hold the service handle */
uint16_t bas_service_handle;

/* Hold the battery level */
uint8_t bas_level = 70;

/* Functions */
static int
gatt_svr_chr_access_bas(uint16_t conn_handle, uint16_t attr_handle,
                               struct ble_gatt_access_ctxt *ctxt, void *arg);

/**
 * @brief battery level service database.
 */
static const struct ble_gatt_svc_def bas_svr_svcs[] = {
    {
        /* Service: Heart-rate */
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(BLE_UUID_BATTERY_SERVICE),
        .characteristics = (struct ble_gatt_chr_def[])
        { 
            {
                /* Characteristic: weight_scale_feature UUID */
                .uuid = BLE_UUID16_DECLARE(BLE_UUID_BATTERY_LEVEL_CHAR),
                .val_handle = &bas_service_handle,
                .access_cb = gatt_svr_chr_access_bas,
                .flags = BLE_GATT_CHR_F_READ,
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
 * @brief Access battery level characteristics.
 * @retval None
 */
static int gatt_svr_chr_access_bas(uint16_t conn_handle, uint16_t attr_handle,
                               struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    uint16_t uuid;
    int rc;

    uuid = ble_uuid_u16(ctxt->chr->uuid);

    switch (uuid)
    {
        case BLE_UUID_BATTERY_LEVEL_CHAR:
        {
            /* ATT req */
            printf("[att req] read battery level \r\n");
            /* Copy location to send buffer */
            rc = os_mbuf_append(ctxt->om, &bas_level, 1);

            break;
        }

        default:
            break;
    }

    rc = (rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES);

    return rc;
}

/**
 * @brief battery level service configuration.
 * @param None
 * @param None
 */
int bas_svr_init(void)
{
    int rc;

    rc = ble_gatts_count_cfg(bas_svr_svcs);
    if (rc != 0) {
        return rc;
    }

    rc = ble_gatts_add_svcs(bas_svr_svcs);
    if (rc != 0) {
        return rc;
    }

    return 0;
}
