#include "power.h"

#include "apps_config.h"

#include "logging.h"
#include "usbpd.h"
#include "stusb4500.h"

#include "cpubsub.h"
#include "messages.h"

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#define LOG_TAG "USBPD"

static MSGUSBPDStats_t usbpd_msg = {0};
static bool usb_attached = false;
static bool tick = false;

static void usbpd_tick(TimerHandle_t timer)
{
	tick = true;
}

void pwr_usbpd_task(void *params)
{
	while (true)
	{
		while (tick == false)
		{
			vTaskDelay(pdMS_TO_TICKS(10));
		}

		tick = false;

		// Update the current status
		usb_attached = stusb_get_attach();

		// If not attached, put charger into idle
		if (usb_attached == false)
		{
			log_info(LOG_TAG, "USB has been detached.\n");
		}

		// Has been attached, get the info
		else
		{
			pdo_t pdo = stusb_read_pdo_selected();
			usbpd_msg.bus_voltage = pdo.voltage;
			usbpd_msg.bus_current = pdo.current;

			log_info(LOG_TAG, "A USB power source has been connected with %2.3fV and %2.3fA.\n", pdo.voltage, pdo.current);
		}

		/* Publish the data */
		usbpd_msg.attached = (uint32_t)usb_attached;
		cps_publish(&usbpd_msg, MSGUSBPDStats_MID);
	}
}

void pwr_usbpd_start(void)
{
	TimerHandle_t usbpd_timer = xTimerCreate("USBPD Tick", pdMS_TO_TICKS(USBPD_TICK_PERIOD_MS), true, NULL, usbpd_tick);
	xTimerStart(usbpd_timer, 0);

	xTaskCreate(pwr_usbpd_task, "USBPD Task", 1024, NULL, tskIDLE_PRIORITY, NULL);
}