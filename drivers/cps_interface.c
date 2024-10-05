#include "cpubsub_network.h"

#include "net_pub.h"
#include "sys_tick.h"

#include "usart.h"

#include "FreeRTOS.h"
#include "task.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

void cps_network_transmit(uint8_t *data, uint32_t len)
{
	// Start code
	const uint8_t start_code[] = "[";
	HAL_UART_Transmit(&huart2, start_code, 1, 1000);

	// Send the payload
	HAL_UART_Transmit(&huart2, data, (uint16_t)len, 100);

	// End code
	const uint8_t end_code[] = "]";
	HAL_UART_Transmit(&huart2, end_code, 1, 1000);
}