#include "net_pub.h"

#include "cpubsub_network.h"

#include "log.h"
#include "usart.h"

// FreeRTOS includes
#include "FreeRTOS.h"
#include "task.h"

// Standard headers
#include <stdbool.h>
#include <string.h>

#define LOG_TAG "NET_PUB"

uint8_t buffer[NET_PUB_BUFF_LEN] = {0};

static void net_pub_init_rx(void)
{
	HAL_UART_DMAStop(&huart2);

	memset(buffer, 0, NET_PUB_BUFF_LEN);

	HAL_UART_Receive_DMA(&huart2, buffer, 13);
}

void net_pub_task(void *params)
{
	while (true)
	{
		// Look for a newline in the buffer
		char *needle = strstr((const char *)buffer, "\n");
		if (needle != NULL)
		{
			uint32_t len = (uint32_t)needle - (uint32_t)buffer;

			log_info(LOG_TAG, "message with length = %u\n", len);

			// Found a new line, give it to the network
			cps_network_recieve((char *)buffer, len);

			// Start a new RX
			net_pub_init_rx();
		}

		vTaskDelay(pdMS_TO_TICKS(10));
	}

	vTaskDelete(NULL);
}
