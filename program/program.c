#include "program.h"

#include "cmsis_os.h"

#include "FreeRTOS.h"
#include "task.h"

#include "power.h"
#include "logging.h"

#define LOG_TAG "PROG"

void blink_task(void * params);

void program_entry()
{
	// Start up stuff
	log_start();
	log_info(LOG_TAG, "Starting system\n");

	// Init the power system
	pwr_start();

	// Get the wake up source
	pwr_wake_source_t wake_source = pwr_get_wake_source();
	switch(wake_source)
	{
	case PWR_WAKE_BUTTON:
		break;

	case PWR_WAKE_USB:
		break;

	case PWR_WAKE_UNKNOWN:
		break;
	}


	xTaskCreate(blink_task, "Blink", 1024, NULL, tskIDLE_PRIORITY, NULL);
}

void sys_delay(uint32_t t_delay_ms)
{
    vTaskDelay(t_delay_ms / portTICK_PERIOD_MS);
}

uint32_t sys_get_tick()
{
    return xTaskGetTickCount();
}

void blink_task(void * params)
{
	while(1)
	{
		HAL_GPIO_WritePin(PMC_LED_RUN_GPIO_Port, PMC_LED_RUN_Pin, true);
		sys_delay(1);
		HAL_GPIO_WritePin(PMC_LED_RUN_GPIO_Port, PMC_LED_RUN_Pin, false);
		sys_delay(99);
	}
}