/**
  *********************************************************************************
  * @file    gatt_svr.c
  * @brief   Heart rate service definition
  * @version V1.0
  * @author  Yuzrain
  *********************************************************************************
  */

#include "FreeRTOS.h"
#include "task.h"
#include "nimble/ble.h"
#include "host/ble_hs.h"
#include "host/ble_uuid.h"
#include "hr_svr.h"
#include "ble_srv_common.h"

/* Hold the service handle */
uint16_t hrs_hrm_handle;

/* Functions */
static int
gatt_svr_chr_access_heart_rate(uint16_t conn_handle, uint16_t attr_handle,
                               struct ble_gatt_access_ctxt *ctxt, void *arg);

/**
 * @brief Heart-rate service database.
 */
static const struct ble_gatt_svc_def hr_svr_svcs[] = {
    {
        /* Service: Heart-rate */
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(BLE_UUID_HEART_RATE_SERVICE),
        .characteristics = (struct ble_gatt_chr_def[]) { {
            /* Characteristic: Heart-rate measurement */
            .uuid = BLE_UUID16_DECLARE(BLE_UUID_HEART_RATE_MEASUREMENT_CHAR),
            .access_cb = gatt_svr_chr_access_heart_rate,
            .val_handle = &hrs_hrm_handle,
            .flags = BLE_GATT_CHR_F_NOTIFY,
        }, {
            /* Characteristic: Body sensor location */
            .uuid = BLE_UUID16_DECLARE(BLE_UUID_BODY_SENSOR_LOCATION_CHAR),
            .access_cb = gatt_svr_chr_access_heart_rate,
            .flags = BLE_GATT_CHR_F_READ,
        }, {
            0, /* No more characteristics in this service */
        }, }
    },

    {
        .type = BLE_GATT_SVC_TYPE_END, /* No more services */
    },
};

/**
 * @brief Access heart-rate.
 * @param None
 * @param None
 */
static int
gatt_svr_chr_access_heart_rate(uint16_t conn_handle, uint16_t attr_handle,
                               struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    /* Sensor location, set to "Chest" */
    static uint8_t body_sens_loc = 0x01;
    uint16_t uuid;
    int rc;

    uuid = ble_uuid_u16(ctxt->chr->uuid);

    if (uuid == BLE_UUID_BODY_SENSOR_LOCATION_CHAR) {
        rc = os_mbuf_append(ctxt->om, &body_sens_loc, sizeof(body_sens_loc));

        return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    }

    assert(0);
    return BLE_ATT_ERR_UNLIKELY;
}

/**
 * @brief Heart-Rate service configuration.
 * @param None
 * @param None
 */
int hr_svr_init(void)
{
    int rc;

    rc = ble_gatts_count_cfg(hr_svr_svcs);
    if (rc != 0) {
        return rc;
    }

    rc = ble_gatts_add_svcs(hr_svr_svcs);
    if (rc != 0) {
        return rc;
    }

    return 0;
}
