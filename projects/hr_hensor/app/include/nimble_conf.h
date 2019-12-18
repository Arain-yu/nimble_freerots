/**
  *********************************************************************************
  * @file    nimble_conf.h
  * @brief   nimble configuration
  * @version V1.0
  * @author  Yuzrain
  *********************************************************************************
  */

#ifndef __NIMBLE_CONF_H__
#define __NIMBLE_CONF_H__

/* Controller configuretion */

#define NIMBLE_CTLR

/**
 * Bluetooth role support
 */
#define MYNEWT_VAL_BLE_ROLE_PERIPHERAL           (1)

#define MYNEWT_VAL_BLE_ROLE_CENTRAL              (1)

#define MYNEWT_VAL_BLE_ROLE_BROADCASTER          (1)

#define MYNEWT_VAL_BLE_ROLE_OBSERVER             (1)

#define MYNEWT_VAL_BLE_MAX_CONNECTIONS           (1)

#define MYNEWT_VAL_BLE_MULTI_ADV_INSTANCES       (0)

#define MYNEWT_VAL_BLE_WHITELIST                 (1)

#define MYNEWT_VAL_BLE_EXT_ADV                   (0)

#define MYNEWT_VAL_BLE_EXT_ADV_MAX_SIZE          (0)

/**
 * Host Stack Configuration
 */ 
#define MYNEWT_VAL_BLE_HOST_THREAD_STACK_SIZE    (1536)

#define MYNEWT_VAL_BLE_HOST_THREAD_PRIORITY      (7)

/**
 * Mesh Configuration
 */ 
#define MYNEWT_VAL_BLE_MESH                      (0)

#define MYNEWT_VAL_BLE_MESH_DEVICE_NAME          

#define MYNEWT_VAL_BLE_MESH_ADV_THREAD_STACK_SIZE (0)

#define MYNEWT_VAL_BLE_MESH_ADV_THREAD_PRIORITY  (0)

#define BLE_MESH_CFG_CLI                         (0)

/**
 * Controller Configuration
 */ 
#define MYNEWT_VAL_BLE_CTLR_THREAD_STACK_SIZE   (1024)

#define MYNEWT_VAL_BLE_CTLR_THREAD_PRIORITY     (6)

#define MYNEWT_VAL_BSP_NRF52                     (1)

#define MYNEWT_VAL_BSP_NRF52840                  (0)

#endif
