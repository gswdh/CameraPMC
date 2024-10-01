#include "cpubsub_network.h"

#include "net_pub.h"
#include "sys_tick.h"

#include "usart.h"

#include "FreeRTOS.h"
#include "task.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

static volatile bool busy = false;

void cps_network_transmit(uint8_t *data, uint32_t len)
{
	while (busy == true)
	{
		SYS_DLY_MS(1);
	}

	busy = true;

	HAL_UART_Transmit(&huart2, data, (uint16_t)len, 100);

	busy = false;
}