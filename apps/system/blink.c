#include "system.h"

#include "gpio.h"

// Drivers
#include "pipe.h"
#include "cpubsub.h"
#include "messages.h"
#include "log.h"
#include "seven_seg.h"

#include "FreeRTOS.h"
#include "timers.h"

#include <stdbool.h>
#include <stdlib.h>

#define LOG_TAG "BLINK"

static pipe_t pipe = {0};

static uint8_t segments_cntr = 0;
const uint8_t segments_loop[6] = {SS_SEG_A, SS_SEG_B, SS_SEG_C, SS_SEG_D, SS_SEG_E, SS_SEG_F};

bool is_charging = false;

void blink_task(TimerHandle_t xTimer)
{
	HAL_GPIO_TogglePin(PMC_LED_RUN_GPIO_Port, PMC_LED_RUN_Pin);

	// uint8_t *msg = (uint8_t *)malloc((size_t)pipe_item_size(&pipe));
	// cps_result_t result = cps_receive(&pipe, (void *)msg, PIPE_WAIT_POLL);
	// if (result == CPS_OK)
	// {
	// 	log_info(LOG_TAG, "Buttons = %08X\n", ((MSGButtonPress_t *)msg)->button_state);
	// }
	// free(msg);

	if (is_charging == true)
	{
		ss_set_segments(segments_loop[segments_cntr++]);

		if (segments_cntr == 6)
		{
			segments_cntr = 0;
		}
	}

	else
	{
		ss_set_segments(0x00);
	}
}

void blink_start(void)
{
	cps_subscribe(MSGButtonPress_MID, MSGButtonPress_LEN, &pipe);

	xTimerHandle timer = xTimerCreate("Blink Timer", pdMS_TO_TICKS(50), true, NULL, blink_task);
	xTimerStart(timer, 0);
}