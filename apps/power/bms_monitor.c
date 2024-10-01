#include "power.h"

#include "apps_config.h"

#include "max17320.h"

#include "cpubsub.h"
#include "messages.h"

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#include <string.h>

#define LOG_TAG "BMS"

static MSGBatteryStats_t msg = {0};

static void bms_tick(TimerHandle_t timer)
{
	// Have we got a BMS interrupt
	if (pwr_bms_nirq() == false)
	{
	}

	bms_stats_t stats = {0};
	bms_get_stats(&stats);

	msg.voltage = stats.volts;
	msg.current = stats.amps;
	msg.soc = stats.soc;
	msg.capacity = stats.cap;
	memcpy((uint8_t *)&msg.status, (uint8_t *)&stats.status, sizeof(uint16_t));
	memcpy((uint8_t *)&msg.faults, (uint8_t *)&stats.fault, sizeof(uint16_t));

	cps_publish((void *)&msg, MSGBatteryStats_MID);
}

void pwr_bms_start(void)
{
	TimerHandle_t bms_timer = xTimerCreate("BMS Tick", pdMS_TO_TICKS(BMS_TICK_PERIOD_MS), true, NULL, bms_tick);
	xTimerStart(bms_timer, 0);
}