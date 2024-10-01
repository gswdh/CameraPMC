#include "logging.h"

#include "usart.h"

#include "sys_tick.h"

#include "FreeRTOS.h"
#include "task.h"

#include <string.h>
#include <stdint.h>
#include <stdbool.h>

static volatile bool busy = false;

void log_start(void)
{
	return;
}

void log_transmit(const char *log)
{
	while (busy == true)
	{
		SYS_DLY_MS(1);
	}

	busy = true;

	HAL_UART_Transmit(&huart1, (uint8_t *)log, strlen(log), 100);

	busy = false;
}

uint32_t log_get_time()
{
	return SYS_TCK_MS();
}
