#pragma once

// CPS Network
#define NET_PUB_BUFF_LEN (1024)

// Debugging
#define DBG_PIPE_LEN (64)
#define DBG_TICK_PERIOD_MS (100)

// Power (general)
#define PWR_PIPE_LEN (8)
#define PWR_TICK_PERIOD_MS (1000)

// Power (charger)
#define CHRG_PIPE_LEN (8)
#define CHRG_TICK_PERIOD_MS (1000)
#define CHRG_STATS_TICK_PERIOD_MS (1000)

// Power (BMS)
#define BMS_TICK_PERIOD_MS (1000)

// Power (USBPD)
#define USBPD_TICK_PERIOD_MS (1000)

// System (general)

// System (blink)
#define BNK_PIPE_LEN (4)
#define BNK_TICK_PERIOD_MS (200)

// System (button)
#define BTN_TICK_PERIOD_MS (100)
#define BTN_HOLD_OFF_T_MS (3000)