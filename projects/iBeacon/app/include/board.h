/**
  *********************************************************************************
  * SPDX-License-Identifier: Apache-2.0
  *
  * @file    board.h
  * @brief   PCA10040 board configuration
  * @version V1.0
  * @author  yuzrain
  *
  *********************************************************************************
  */

#include "pca10040.h"

/* Define the LED pin */
#define LED_1          17
#define LED_2          18
#define LED_3          19
#define LED_4          20

/* Define LED mask */
#define BOARD_LED1_LIGHT    0x00000001U
#define BOARD_LED2_LIGHT    0x00000002U
#define BOARD_LED3_LIGHT    0x00000004U
#define BOARD_LED4_LIGHT    0x00000008U

/* External function */
void board_init(void);
