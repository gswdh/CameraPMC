#include "system.h"

#include "FreeRTOS.h"
#include "task.h"

#include "power.h"
#include "logging.h"

#define LOG_TAG "SYS"

void system_start()
{
	// Start up stuff
	log_info(LOG_TAG, "Starting system\n");

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

	case PWR_WAKE_UNKNOWN:
		log_info(LOG_TAG, "Wake source is unknown.\n");
		break;
	}

	// Activity status
	blink_start();
	button_start();

	vTaskDelete(NULL);
}
