#include "power.h"

#include "apps_config.h"

#include "charger.h"
#include "system.h"
#include "logging.h"
#include "usbpd.h"
#include "gpio.h"
#include "stusb4500.h"

#include "cpubsub.h"
#include "messages.h"

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#include <stdlib.h>
#include <assert.h>

#define LOG_TAG "PWR"

static pwr_state_t pwr_state = PWR_STATE_UNINIT;

static pipe_t pipe = {0};
static uint8_t *buffer = NULL;

static bool charging = false;

static char *pwr_state_str[] = {"unintialised", "off", "charging", "run"};
static char *pwr_get_state_str(pwr_state_t state)
{
	switch (state)
	{
	case PWR_STATE_OFF:
		return pwr_state_str[1];
		break;
	case PWR_STATE_UNINIT:
		return pwr_state_str[0];
		break;
	case PWR_STATE_CHARGING:
		return pwr_state_str[2];
		break;
	case PWR_STATE_RUN:
		return pwr_state_str[3];
		break;
	default:
		return NULL;
		break;
	}
}

static void pwr_set_pwr_state(pwr_state_t state)
{
	log_info(LOG_TAG, "State %s to %s.\n", pwr_get_state_str(pwr_state), pwr_get_state_str(state));

	switch (pwr_state)
	{
	case PWR_STATE_UNINIT:
		switch (state)
		{
		case PWR_STATE_OFF:
			pwr_sys_off();
			pwr_sleep();
			pwr_state = state;
			break;
		case PWR_STATE_CHARGING:
			pwr_state = state;
			break;
		case PWR_STATE_RUN:
			pwr_sys_on();
			pwr_state = state;
			break;
		default:
			log_warn(LOG_TAG, "Invalid from / to power states.\n");
			break;
		}
		break;

	case PWR_STATE_CHARGING:
		switch (state)
		{
		case PWR_STATE_OFF:
			pwr_sys_off();
			pwr_sleep();
			pwr_state = state;
			break;
		case PWR_STATE_RUN:
			pwr_sys_on();
			pwr_state = state;
			break;
		default:
			log_warn(LOG_TAG, "Invalid from / to power states.\n");
			break;
		}
		break;

	case PWR_STATE_RUN:
		switch (state)
		{
		case PWR_STATE_OFF:
			pwr_sys_off();
			pwr_state = PWR_STATE_CHARGING;
			if (pwr_is_charging() == false)
			{
				pwr_sleep();
			}
			break;
		case PWR_STATE_CHARGING:
			pwr_sys_off();
			pwr_state = state;
			break;

		default:
			log_warn(LOG_TAG, "Invalid from / to power states.\n");
			break;
		}
		break;
	default:
		log_error(LOG_TAG, "Currently in unknown power state, turning off.\n");
		pwr_sys_off();
		pwr_sleep();
		break;
	}
}

static void pwr_tick(TimerHandle_t timer)
{
	assert(buffer != NULL && "pwr_tick buffer is NULL\n");

	// Handle power button presses
	if (cps_receive(&pipe, (void *)buffer, PIPE_WAIT_POLL) == CPS_OK)
	{
		topic_t mid = cps_get_mid((void *)buffer);

		// If we are the right message type
		if (mid == MSGPowerButton_MID)
		{
			// Handle button press conditions
			switch (((MSGPowerButton_t *)buffer)->state)
			{
			case GPIO_RISING:
				pwr_set_pwr_state(PWR_STATE_RUN);
				break;
			case GPIO_FALLING:
				// Don't care
				break;
			case GPIO_LONG:
				pwr_set_pwr_state(PWR_STATE_OFF);
				break;
			default:
				break;
			}
		}
	}

	// If charing stops
	if (pwr_is_charging() != charging)
	{
		// Update the status
		charging = pwr_is_charging();

		// If charging has stopped
		if (charging == false)
		{
			// If the system is only running for charging and it's stopped, turn the system off
			if (pwr_state == PWR_STATE_CHARGING)
			{
				pwr_set_pwr_state(PWR_STATE_OFF);
			}
		}
	}

	// Send some telemetry
	MSGSystemStats_t status = {0};
	status.bus_voltage = pwr_measure_voltage_V();
	status.bus_current = pwr_measure_current_A();
	status.bus_power = status.bus_current * status.bus_voltage;
	status.pmc_temp = pwr_measure_power_T();

	cps_publish((void *)&status, MSGSystemStats_MID);
}

void pwr_start()
{
	// Set the initial power state
	pwr_state = PWR_STATE_UNINIT;

	// Subscribe to button presses
	pipe_set_length(&pipe, PWR_PIPE_LEN);
	cps_subscribe(MSGPowerButton_MID, MSGPowerButton_LEN, &pipe);

	// Make some room for RXd packets
	buffer = (uint8_t *)malloc(pipe_item_size(&pipe));
	if (buffer == NULL)
	{
		log_error(LOG_TAG, "Was unable to malloc for cps.\n");
	}

	// Default non charging state for the charger
	CHRG_EnterHiZ();

	// Init the USB PD
	usbpd_start();

	// Start the ADC for power measurement stats
	pwr_measure_start();

	// Get the wake up source
	switch (pwr_get_wake_source())
	{
	case PWR_WAKE_BUTTON:
		log_info(LOG_TAG, "Wake source is the button.\n");
		pwr_set_pwr_state(PWR_STATE_RUN);
		break;

	case PWR_WAKE_USB:
		log_info(LOG_TAG, "Wake source is the USB.\n");
		pwr_set_pwr_state(PWR_STATE_CHARGING);
		break;

	case PWR_WAKE_UNKNOWN:
		log_info(LOG_TAG, "Wake source is unknown.\n");
		pwr_set_pwr_state(PWR_STATE_OFF);
		break;
	}

	TimerHandle_t pwr_timer = xTimerCreate("Power Tick", pdMS_TO_TICKS(PWR_TICK_PERIOD_MS), true, NULL, pwr_tick);
	xTimerStart(pwr_timer, 0);

	// Start the power control threads
	pwr_usbpd_start();
	pwr_chrg_start();
	pwr_bms_start();

	// Fall off the end
	vTaskDelete(NULL);
}
