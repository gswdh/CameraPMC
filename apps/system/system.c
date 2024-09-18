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

	// Activity status
	blink_start();
	button_start();

	vTaskDelete(NULL);
}
