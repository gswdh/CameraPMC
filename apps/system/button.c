#include "system.h"

#include "gpio.h"
#include "sys_tick.h"

#include "power.h"
#include "logging.h"
#include "seven_seg.h"

#include "FreeRTOS.h"
#include "timers.h"

#include <stdint.h>
#include <stdbool.h>

#define LOG_TAG "BTTN"

static uint32_t t_button_pressed_ms = 0;

void button_task(TimerHandle_t xTimer)
{
	// If the button isn't pressed just return
	if (!HAL_GPIO_ReadPin(BTN_THUMB_GPIO_Port, BTN_THUMB_Pin))
	{
		// Keepthe timestamp reset
		t_button_pressed_ms = 0;
		return;
	}

	// This is a button rising edge
	if (t_button_pressed_ms == 0)
	{
		t_button_pressed_ms = SYS_TCK_MS();
	}

	// Continued pressing
	if (SYS_TCK_MS() > (t_button_pressed_ms + T_OFF_BUTTON_TO_MS))
	{
		log_info(LOG_TAG, "Turning system off, goodbye!\n");

		// Set the LED indicator
		HAL_GPIO_WritePin(PMC_LED_STATUS_GPIO_Port, PMC_LED_STATUS_Pin, 1);

		// Turn the system off
		pwr_sys_off();

		// Wait for that to happen
		SYS_DLY_MS(100);

		// Wait for the button to be released
		while (HAL_GPIO_ReadPin(BTN_THUMB_GPIO_Port, BTN_THUMB_Pin))
		{
			SYS_DLY_MS(10);
		}

		// Turn the SS off
		ss_set_segments(0x00);

		// Sleep the system
		pwr_sleep();
	}
}

void button_start(void)
{
	xTimerHandle timer = xTimerCreate("Button Timer", pdMS_TO_TICKS(100), true, NULL, button_task);
	xTimerStart(timer, 0);
}