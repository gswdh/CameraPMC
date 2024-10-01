#include "system.h"

#include "apps_config.h"

#include "pipe.h"
#include "cpubsub.h"
#include "messages.h"
#include "log.h"
#include "seven_seg.h"
#include "gpio.h"

#include "FreeRTOS.h"
#include "timers.h"

#include <stdbool.h>
#include <stdlib.h>

#define LOG_TAG "BLINK"

static pipe_t pipe = {0};
static uint8_t *buffer = NULL;

bool is_charging = false;

void blink_task(TimerHandle_t xTimer)
{
	HAL_GPIO_TogglePin(PMC_LED_RUN_GPIO_Port, PMC_LED_RUN_Pin);

	cps_result_t result = cps_receive(&pipe, (void *)buffer, PIPE_WAIT_POLL);
	if (result == CPS_OK)
	{
		if (cps_get_mid((void *)buffer) == MSGChargingStats_MID)
		{
			if (((MSGChargingStats_t *)buffer)->charging == 0)
			{
				ss_set_segments(SS_SEG_A | SS_SEG_B | SS_SEG_C | SS_SEG_D | SS_SEG_E | SS_SEG_F);
			}

			else
			{
				ss_set_segments(SS_SEG_A | SS_SEG_D | SS_SEG_E | SS_SEG_F);
			}
		}
	}
}

void blink_start(void)
{
	pipe_set_length(&pipe, BNK_PIPE_LEN);
	cps_subscribe(MSGChargingStats_MID, MSGChargingStats_LEN, &pipe);

	buffer = (uint8_t *)malloc((size_t)pipe_item_size(&pipe));
	if (buffer == NULL)
	{
		log_error(LOG_TAG, "malloc failed\n");
		return;
	}

	xTimerHandle timer = xTimerCreate("Blink Timer", pdMS_TO_TICKS(BNK_TICK_PERIOD_MS), true, NULL, blink_task);
	xTimerStart(timer, 0);
}