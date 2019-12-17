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
#include "gap_svr.h"
#include "ble_srv_common.h"

/* Hold the device name */
extern const char user_ble_device_name[];
/* Hold the device appearance */
extern uint16_t user_ble_appearance;

uint16_t gap_svr_handle;

static int
gatt_svr_chr_access_device_info(uint16_t conn_handle, uint16_t attr_handle,
                                struct ble_gatt_access_ctxt *ctxt, void *arg);

static const struct ble_gatt_svc_def gap_svr_svcs[] = {
    {
        /* Service: Heart-rate */
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(BLE_UUID_GENERAL_ACCESS_SERVICE),
        .characteristics = (struct ble_gatt_chr_def[]) { {
            /* Characteristic: Heart-rate measurement */
            .uuid = BLE_UUID16_DECLARE(BLE_UUID_DEVICE_NAME_CHAR),
            .access_cb = gatt_svr_chr_access_device_info,
            .val_handle = &gap_svr_handle,
            .flags = BLE_GATT_CHR_F_READ,
        }, {
            /* Characteristic: Body sensor location */
            .uuid = BLE_UUID16_DECLARE(BLE_UUID_APPEARANCE_CHAR),
            .access_cb = gatt_svr_chr_access_device_info,
            .flags = BLE_GATT_CHR_F_READ,
        }, {
            0, /* No more characteristics in this service */
        }, }
    },
    {
        .type = BLE_GATT_SVC_TYPE_END, /* No more services */
    }
};

/**
 * @brief Heart-Rate service configuration.
 * @param None
 * @param None
 */
static int
gatt_svr_chr_access_device_info(uint16_t conn_handle, uint16_t attr_handle,
                                struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    uint16_t uuid;
    int rc;

    uuid = ble_uuid_u16(ctxt->chr->uuid);

    if (uuid == BLE_UUID_DEVICE_NAME_CHAR) {
        rc = os_mbuf_append(ctxt->om, user_ble_device_name, strlen(user_ble_device_name));
        return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    }

    if (uuid == BLE_UUID_APPEARANCE_CHAR) {
        rc = os_mbuf_append(ctxt->om, &user_ble_appearance, 2);
        return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    }

    assert(0);
    return BLE_ATT_ERR_UNLIKELY;
}

/**
 * @brief GAP service configuration.
 * @param None
 * @param None
 */
int gap_svr_init(void)
{
    int rc;

    rc = ble_gatts_count_cfg(gap_svr_svcs);
    if (rc != 0) {
        return rc;
    }

    rc = ble_gatts_add_svcs(gap_svr_svcs);
    if (rc != 0) {
        return rc;
    }

    return 0;
}
