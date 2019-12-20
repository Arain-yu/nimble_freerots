/**
  *********************************************************************************
  * SPDX-License-Identifier: Apache-2.0
  *
  * @file    gatt_svr.c
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
#include "hr_svr.h"
#include "ble_srv_common.h"
#include "nimble/nimble_port.h"

/* Hold the service handle */
uint16_t hrs_hrm_service_handle;

/* Hold the current notify connection handle */
static uint16_t notify_conn_handle;

/* Hold the current notify state */
static bool notify_state;

/* Hold current heart beats */
static uint8_t heartrate = 90;

/* Hold heart-rate tx timer */
struct ble_npl_callout hr_tx_timer;

/* Functions */
static int
gatt_svr_chr_access_heart_rate(uint16_t conn_handle, uint16_t attr_handle,
                               struct ble_gatt_access_ctxt *ctxt, void *arg);

static int
gatt_svr_chr_access_hr_des(uint16_t conn_handle, uint16_t attr_handle,
                               struct ble_gatt_access_ctxt *ctxt, void *arg);

/* Add BLE_UUID_HEART_RATE_MEASUREMENT_CHAR description */
static int8_t des_hr[] = "heart rate";
struct ble_gatt_dsc_def hr_me_user_des = {
    .uuid       = BLE_UUID16_DECLARE(0x2901),
    .att_flags  = BLE_ATT_F_READ,
    .access_cb  = gatt_svr_chr_access_hr_des,
};

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
            .val_handle = &hrs_hrm_service_handle,
            .flags = BLE_GATT_CHR_F_NOTIFY,
            .descriptors = &hr_me_user_des,
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

static int
gatt_svr_chr_access_hr_des(uint16_t conn_handle, uint16_t attr_handle,
                               struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    os_mbuf_append(ctxt->om, des_hr, sizeof(des_hr));
    return 0;
}

/**
 * @brief Access heart-rate.
 * @param None
 * @param None
 */
static int
gatt_svr_chr_access_heart_rate(uint16_t conn_handle, uint16_t attr_handle,
                               struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    /* Sensor location, set to "Hand" */
    uint8_t body_sens_loc = BLE_BODY_SENSOR_LOCATION_CHAR_HAND;
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
 * @brief Stop notifying heart rate values.
 * @param None
 * @param None
 */
static void
hr_tx_hrate_stop(void)
{
    ble_npl_callout_stop(&hr_tx_timer);
}

/**
 * @brief Start notifying heart rate values
 * @note  This function will
 * @param None
 * @param None
 */
static void
hr_tx_hrate_start(void)
{
    int rc;

    /* 1S/1-notify */
    rc = ble_npl_callout_reset(&hr_tx_timer, configTICK_RATE_HZ);
    assert(rc == 0);
}

/**
 * @brief Heart-rate subscribe handler.
 * @param None
 * @param None
 */
void hr_service_subscribe_handler(struct ble_gap_event *event, void *arg)
{
    if (event->type != BLE_GAP_EVENT_SUBSCRIBE)
        return;

    printf(":hr subscribe event(cur_notify:%d | service handle=%d)\r\n",
                    event->subscribe.cur_notify, event->subscribe.attr_handle);

    if (event->subscribe.attr_handle == hrs_hrm_service_handle)
    {
        notify_state = event->subscribe.cur_notify;
        notify_conn_handle = event->subscribe.conn_handle;
        hr_tx_hrate_start();
    } else if (event->subscribe.attr_handle != hrs_hrm_service_handle) 
    {
        notify_state = event->subscribe.cur_notify;
        notify_conn_handle = 0;
        hr_tx_hrate_stop();
    }
}

/**
 * @brief Send heart-Rate noyify.
 * @param None
 * @param None
 */
static void hr_tx_hrate(struct ble_npl_event *ev)
{
    static uint8_t hrm[2];
    int rc;
    struct os_mbuf *om;

    if (!notify_state) {
        hr_tx_hrate_stop();
        heartrate = 90;
        return;
    }

    hrm[0] = 0x06; /* contact of a sensor */
    hrm[1] = heartrate; /* storing dummy data */

    /* Simulation of heart beats */
    heartrate++;
    if (heartrate == 110) {
        heartrate = 90;
    }

    om = ble_hs_mbuf_from_flat(hrm, sizeof(hrm));

    rc = ble_gattc_notify_custom(notify_conn_handle, hrs_hrm_service_handle, om);

    assert(rc == 0);
    hr_tx_hrate_start();
}

/**
 * @brief Heart-Rate service configuration.
 * @param None
 * @param None
 */
int hr_svr_init(void)
{
    int rc;

    /* Config heart-rate notify timer 1S/1-notify */
    ble_npl_callout_init(&hr_tx_timer, 
                        nimble_port_get_dflt_eventq(),
                        hr_tx_hrate, NULL);

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
