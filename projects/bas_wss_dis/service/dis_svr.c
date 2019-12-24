/**
  *********************************************************************************
  * SPDX-License-Identifier: Apache-2.0
  *
  * @file    dis_svr.c
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
#include "dis_svr.h"
#include "ble_srv_common.h"
#include "nimble/nimble_port.h"

/* Hold the service handle */
uint16_t dis_service_handle;

/* Hold the Manufacturer name string */
static const char manufacturee_name_str[]="YUZRAIN";

/* Functions */
static int
gatt_svr_chr_access_dis(uint16_t conn_handle, uint16_t attr_handle,
                               struct ble_gatt_access_ctxt *ctxt, void *arg);

/**
 * @brief Device Information service database.
 */
static const struct ble_gatt_svc_def dis_svr_svcs[] = {
    {
        /* Service: Heart-rate */
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(BLE_UUID_DEVICE_INFORMATION_SERVICE),
        .characteristics = (struct ble_gatt_chr_def[])
        { 
            {
                /* Characteristic: Manufacturer Name String UUID */
                .uuid = BLE_UUID16_DECLARE(BLE_UUID_MANUFACTURER_NAME_STRING_CHAR),
                .access_cb = gatt_svr_chr_access_dis,
                .val_handle = &dis_service_handle,
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
 * @brief Access Device Information characteristics.
 * @retval None
 */
static int gatt_svr_chr_access_dis(uint16_t conn_handle, uint16_t attr_handle,
                               struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    uint8_t hr_data[2];
    uint16_t uuid;
    int rc;

    uuid = ble_uuid_u16(ctxt->chr->uuid);

    switch (uuid)
    {
        case BLE_UUID_MANUFACTURER_NAME_STRING_CHAR:
        {
            /* ATT req */
            printf("[att req] read manufacturer name \r\n");
            /* Copy location to send buffer */
            rc = os_mbuf_append(ctxt->om,
                                &manufacturee_name_str,
                                sizeof(manufacturee_name_str));

            break;
        }

        default:
            break;
    }

    rc = (rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES);

    return rc;
}

/**
 * @brief Device Information service configuration.
 * @param None
 * @param None
 */
int dis_svr_init(void)
{
    int rc;

    rc = ble_gatts_count_cfg(dis_svr_svcs);
    if (rc != 0) {
        return rc;
    }

    rc = ble_gatts_add_svcs(dis_svr_svcs);
    if (rc != 0) {
        return rc;
    }

    return 0;
}
