#include "power.h"

#include "apps_config.h"

#include "logging.h"
#include "act2861.h"
#include "charger.h"

#include "cpubsub.h"
#include "messages.h"

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#define LOG_TAG "CHRG"

static pipe_t pipe = {0};
static MSGChargingStats_t charging_msg = {0};
static MSGUSBPDStats_t usbpd_msg = {0};
static bool usb_attached = false;
static bool tick = false;

bool pwr_is_charging()
{
	return (charging_msg.charging == 0U) ? false : true;
}

static void chg_stats_tick(TimerHandle_t timer)
{
	CHRG_ADCResults results = {0};
	CHRG_GetADCResults(&results);

	/* Publish the data */
	charging_msg.input_voltage = results.v_in_volts;
	charging_msg.input_current = results.i_in_amps;
	charging_msg.output_voltage = results.v_bat_volts;
	charging_msg.output_current = results.i_bat_amps;
	charging_msg.chrgr_temp = results.t_die_celcius;
	cps_publish(&charging_msg, MSGChargingStats_MID);
}

static void chg_tick(TimerHandle_t timer)
{
	tick = true;
}

void pwr_chg_task(void *params)
{
	while (true)
	{
		while (tick == false)
		{
			vTaskDelay(pdMS_TO_TICKS(10));
		}

		tick = false;

		// Check the charging status here now
		act_error_t status = CHRG_VerifyCHGMode();
		charging_msg.charging = (status == ACT_CHG_IN_PROGRESS) ? 1U : 0U;

		// Look for a USB PD message for any news
		cps_result_t res = cps_receive(&pipe, (void *)&usbpd_msg, PIPE_WAIT_POLL);
		if (res != CPS_OK)
		{
			// No message, give up here
			continue;
		}

		// Check to see if the USB has been plugged in or out
		if (usb_attached == usbpd_msg.attached)
		{
			// Nothing has changed
			continue;
		}

		// Update the current status
		usb_attached = usbpd_msg.attached;

		// If not attached, put charger into idle
		if (usb_attached == false)
		{
			CHRG_EnterHiZ();
			log_info(LOG_TAG, "USB has been detached, stopped charging.\n");
			continue;
		}

		// Check if there's a pack attached
		if (pwr_has_battery() == false)
		{
			log_info(LOG_TAG, "No battery pack detected, will not attempt to charge.\n");
			continue;
		}

		act_error_t error = CHRG_EnableCharging(PWR_MAX_CHARGE_CURRENT_A, usbpd_msg.bus_current);
		if (error == ACT_OK)
		{
			log_info(LOG_TAG, "USB has been attached. Started charging with a %2.3fA input current limit.\n", usbpd_msg.bus_current);
		}

		else
		{
			log_info(LOG_TAG, "USB has been attached. Start charging failed with code = %u. Moving to idle.\n", error);
			CHRG_EnterHiZ();
		}
	}
}

void pwr_chrg_start(void)
{
	pipe_set_length(&pipe, CHRG_PIPE_LEN);
	cps_subscribe(MSGUSBPDStats_MID, MSGUSBPDStats_LEN, &pipe);

	TimerHandle_t chg_timer = xTimerCreate("Charger Tick", pdMS_TO_TICKS(CHRG_TICK_PERIOD_MS), true, NULL, chg_tick);
	xTimerStart(chg_timer, 0);

	TimerHandle_t chg_stats_timer = xTimerCreate("Charger Stats Tick", pdMS_TO_TICKS(CHRG_STATS_TICK_PERIOD_MS), true, NULL, chg_stats_tick);
	xTimerStart(chg_stats_timer, 0);

	xTaskCreate(pwr_chg_task, "Charger Task", 1024, NULL, tskIDLE_PRIORITY, NULL);
}