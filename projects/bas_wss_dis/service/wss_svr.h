/**
  *********************************************************************************
  * SPDX-License-Identifier: Apache-2.0
  *
  * @file    wss_svr.h
  * @brief   Heart rate service definition
  * @version V1.0
  * @author  yuzrain
  *
  *********************************************************************************
  */

#ifndef __WSS_SVR_H__
#define __WSS_SVR_H__

/* Device Information service init */
int wss_svr_init(void);
void wss_service_subscribe_handler(struct ble_gap_event *event, void *arg);

#endif

