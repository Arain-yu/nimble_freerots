/**
  *********************************************************************************
  * @file    gatt_svr.h
  * @brief   Heart rate service definition
  * @version V1.0
  * @author  Yuzrain
  *********************************************************************************
  */

#ifndef __HR_SVR_H__
#define __HR_SVR_H__

/* Body sensor location */
#define BLE_BODY_SENSOR_LOCATION_CHAR_OTHER     0x00
#define BLE_BODY_SENSOR_LOCATION_CHAR_CHEST     0x01
#define BLE_BODY_SENSOR_LOCATION_CHAR_WRIST     0x02
#define BLE_BODY_SENSOR_LOCATION_CHAR_FINGER    0x03
#define BLE_BODY_SENSOR_LOCATION_CHAR_HAND      0x04
#define BLE_BODY_SENSOR_LOCATION_CHAR_EAR_LOBE  0x05
#define BLE_BODY_SENSOR_LOCATION_CHAR_FOOT      0x06

/* Heart-rate service init */
int hr_svr_init(void);
void hr_service_subscribe_handler(struct ble_gap_event *event, void *arg);

#endif

