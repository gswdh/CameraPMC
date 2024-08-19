#include "logging.h"

#include "usart.h"

#include "FreeRTOS.h"
#include "task.h"

#include <string.h>
#include <stdint.h>

void log_start(void)
{
	return;
}

void log_transmit(const char *log)
{
	HAL_UART_Transmit(&huart1, (uint8_t *)log, strlen(log), 100);
}

uint32_t log_get_time()
{
	return xTaskGetTickCount();
}
