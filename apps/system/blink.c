#include "system.h"

#include "gpio.h"

#include "FreeRTOS.h"
#include "timers.h"

#include <stdbool.h>

void blink_task(TimerHandle_t xTimer)
{
	HAL_GPIO_TogglePin(PMC_LED_RUN_GPIO_Port, PMC_LED_RUN_Pin);
}

void blink_start(void)
{
	xTimerHandle timer = xTimerCreate("Blink Timer", pdMS_TO_TICKS(50), true, NULL, blink_task);
	xTimerStart(timer, 0);
}