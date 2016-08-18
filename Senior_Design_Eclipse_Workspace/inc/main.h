// Adapted from STemWin/STemWin_HelloWorld/Inc/main.h.  See LICENSE.

#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal.h"
#include "GUI.h"
#include "stm32746g_discovery_ts.h"
#include "stm32746g_discovery_lcd.h"
#include "stm32746g_discovery_qspi.h"
#include "WM.h"
#include "BUTTON.h"

/* EVAL includes component */
#include "stm32746g_discovery.h"
#include "stm32746g_discovery_sdram.h"
#include "bsp.h"
#include "motor.h"
#include "encoder.h"
#include "System_Variable_Settings.h"

#endif /* __MAIN_H */

extern int32_t VerticalCount, ClawCount;//, DeltaVerticalCount;
