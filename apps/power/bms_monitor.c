#include "power.h"

#include "max17320.h"

#include "cpubsub.h"
#include "messages.h"

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#define LOG_TAG "BMS"

static MSGBatteryStats_t bms_msg = {0};

static void bms_tick(TimerHandle_t timer)
{
	bms_stats_t stats = {0};
	bms_get_stats(&stats);

	bms_msg.voltage = stats.volts;
	bms_msg.current = stats.amps;
	bms_msg.soc = stats.soc;

	cps_publish((void *)&bms_msg, MSGBatteryStats_MID);
}

void pwr_bms_start(void)
{
	TimerHandle_t bms_timer = xTimerCreate("BMS Tick", pdMS_TO_TICKS(1000), true, NULL, bms_tick);
	xTimerStart(bms_timer, 0);
}