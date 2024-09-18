#include "cpubsub_network.h"

#include "net_pub.h"

#include "usart.h"
#include "log.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define LOG_TAG "CPS_INT"

static bool busy = false;

void cps_network_transmit(uint8_t *data, uint32_t len)
{
	// char msg[256] = {0};
	// for (uint32_t i = 0; i < len; i++)
	// {
	// 	int n = strlen(msg);
	// 	snprintf(msg + n, sizeof(msg) - n, "%02X ", data[i]);
	// }

	// log_info(LOG_TAG, "[%u] %.*s\n", len, (len * 3) - 1, msg);

	while (busy == true)
	{
	}

	busy = true;

	HAL_UART_Transmit(&huart2, data, (uint16_t)len, 100);

	busy = false;
}