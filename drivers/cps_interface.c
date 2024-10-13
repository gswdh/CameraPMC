#include "cpubsub_network.h"

#include "net_pub.h"
#include "sys_tick.h"

#include "usart.h"

#include <string.h>
#include <stdio.h>

void cps_network_transmit(uint8_t *data, uint32_t len)
{
	uint8_t payload[256] = {0};
	sprintf((char *)payload, "[%s]", (char *)data);

	// Send the payload
	HAL_UART_Transmit(&huart2, payload, (uint16_t)strlen((const char *)payload), 100);
}
