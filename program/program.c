#include "program.h"

#include "cmsis_os.h"

#include "FreeRTOS.h"
#include "task.h"

#include "power.h"
#include "logging.h"

#include "gpio.h"

#define LOG_TAG "PROG"

void blink_task(void *params);
void button_task(void *params);

void program_entry()
{
	// Start up stuff
	log_start();
	log_info(LOG_TAG, "Starting system\n");

	// Init the power system
	pwr_start();

	// Power the system up
	pwr_sys_on();

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
	xTaskCreate(blink_task, "Blink", 1024, NULL, tskIDLE_PRIORITY, NULL);
	//xTaskCreate(button_task, "Button", 1024, NULL, tskIDLE_PRIORITY, NULL);
}

void sys_delay(uint32_t t_delay_ms)
{
	vTaskDelay(t_delay_ms / portTICK_PERIOD_MS);
}

uint32_t sys_get_tick()
{
	return xTaskGetTickCount();
}

void blink_task(void *params)
{
	while (1)
	{
		HAL_GPIO_WritePin(PMC_LED_RUN_GPIO_Port, PMC_LED_RUN_Pin, true);
		sys_delay(1);
		HAL_GPIO_WritePin(PMC_LED_RUN_GPIO_Port, PMC_LED_RUN_Pin, false);
		sys_delay(99);
	}
}

void button_task(void *params)
{
	uint32_t tick = sys_get_tick();

	while (1)
	{
		if (sys_get_tick() > (tick + 1000))
		{
			tick = sys_get_tick();

			// Stay in the loop while the button is held
			while (HAL_GPIO_ReadPin(SYS_WAKE_GPIO_Port, SYS_WAKE_Pin))
			{
				// If it's held longer than 2s
				if (sys_get_tick() > tick + 2000)
				{
					// Set the LED indicator
					HAL_GPIO_WritePin(PMC_LED_STATUS_GPIO_Port, PMC_LED_STATUS_Pin, 1);

					// Wait for the button to be released
					while (HAL_GPIO_ReadPin(SYS_WAKE_GPIO_Port, SYS_WAKE_Pin))
					{
						sys_delay(10);
					}

					// Turn the system off
					pwr_sys_off();

					// Wait for that to happen
					sys_delay(100);

					// Sleep the system
					pwr_sleep();
				}
			}
		}
	}
}