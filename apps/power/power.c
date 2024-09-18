#include "power.h"

#include "charger.h"
#include "system.h"
#include "logging.h"
#include "usbpd.h"

#include "cpubsub.h"
#include "messages.h"

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#define LOG_TAG "PWR"

static void pwr_tick(TimerHandle_t timer)
{
	MSGSystemStats_t status = {0};
	status.bus_voltage = pwr_measure_voltage_V();
	status.bus_current = pwr_measure_current_A();
	status.bus_power = status.bus_current * status.bus_voltage;
	status.pmc_temp = pwr_measure_power_T();

	cps_publish((void *)&status, MSGSystemStats_MID);
}

void pwr_start()
{
	// Default non charging state for the charger
	CHRG_EnterHiZ();

	// Init the USB PD
	usbpd_start();

	// Start the ADC for power measurement stats
	pwr_measure_start();

	// Get the wake up source
	pwr_wake_source_t wake_source = pwr_get_wake_source();
	switch (wake_source)
	{
	case PWR_WAKE_BUTTON:
		log_info(LOG_TAG, "Wake source is the button.\n");
		break;

	case PWR_WAKE_USB:
		log_info(LOG_TAG, "Wake source is the USB.\n");
		break;

	case PWR_WAKE_BMS:
		log_info(LOG_TAG, "Wake source is the BMS.\n");
		break;

	case PWR_WAKE_UNKNOWN:
		log_info(LOG_TAG, "Wake source is unknown.\n");
		// Go to sleep
		pwr_sleep();
		break;
	}

	// Power the system up
	pwr_sys_on();

	TimerHandle_t pwr_timer = xTimerCreate("Power Tick", pdMS_TO_TICKS(1000), true, NULL, pwr_tick);
	xTimerStart(pwr_timer, 0);

	// Start the power control threads
	pwr_usbpd_start();
	pwr_chrg_start();
	pwr_bms_start();

	// Fall off the end
	vTaskDelete(NULL);
}
