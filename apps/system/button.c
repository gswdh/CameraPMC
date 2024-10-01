#include "system.h"

#include "apps_config.h"

#include "gpio.h"
#include "sys_tick.h"
#include "power.h"
#include "logging.h"
#include "seven_seg.h"

#include "cpubsub.h"
#include "messages.h"

#include "FreeRTOS.h"
#include "timers.h"

#include <stdint.h>
#include <stdbool.h>

#define LOG_TAG "BTN"

static uint32_t t_button_pressed_ms = 0;

static MSGPowerButton_t msg = {0};

void button_task(TimerHandle_t xTimer)
{
	// If the button isn't pressed just return
	if (HAL_GPIO_ReadPin(BTN_THUMB_GPIO_Port, BTN_THUMB_Pin) == false)
	{
		// A falling edge
		if (t_button_pressed_ms != 0)
		{
			// Let everyone know
			msg.state = (uint32_t)GPIO_FALLING;
			cps_publish((void *)&msg, MSGPowerButton_MID);

			// Keep the time stamp reset
			t_button_pressed_ms = 0;
		}

		return;
	}

	// This is a rising edge
	if (t_button_pressed_ms == 0)
	{
		// Send a rising edge
		msg.state = (uint32_t)GPIO_RISING;
		cps_publish((void *)&msg, MSGPowerButton_MID);

		// Get the rising edge time stamp
		t_button_pressed_ms = SYS_TCK_MS();
	}

	// Continued pressing
	if (SYS_TCK_MS() > (t_button_pressed_ms + BTN_HOLD_OFF_T_MS))
	{
		// Set the LED indicator
		HAL_GPIO_WritePin(PMC_LED_STATUS_GPIO_Port, PMC_LED_STATUS_Pin, 1);

		// Wait for the button to be released
		while (HAL_GPIO_ReadPin(BTN_THUMB_GPIO_Port, BTN_THUMB_Pin))
		{
			SYS_DLY_MS(10);
		}

		msg.state = (uint32_t)GPIO_LONG;
		cps_publish((void *)&msg, MSGPowerButton_MID);
	}
}

void button_start(void)
{
	xTimerHandle timer = xTimerCreate("Button Timer", pdMS_TO_TICKS(BTN_TICK_PERIOD_MS), true, NULL, button_task);
	xTimerStart(timer, 0);
}