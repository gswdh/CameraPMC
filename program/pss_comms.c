#include "pss_comms.h"

#include "cmsis_os.h"

#include "logging.h"
#include "base64.h"
#include "program.h"

#include "cpubsub.h"
#include "messages.h"

#include "usart.h"

#include <stdlib.h>
#include <assert.h>

static pipe_t pipe = {0};

#define LOG_TAG "PSS_COMMS"

#define DATA_LEN (64)

static char encoded_data[256] = {0};

static uint32_t pss_comms_encode_data(uint8_t *data, uint32_t len)
{
	/* Calc the encoded data length */
	uint32_t encoded_data_len = BASE64_ENCODE_OUT_SIZE(len);

	/* Encode and add a terminator */
	base64_encode(data, len, encoded_data);

	/* Return the length for TX */
	return encoded_data_len + 1;
}

static void pss_comms_send(uint8_t *data, uint32_t data_len)
{
	uint32_t len = pss_comms_encode_data(data, data_len);
	HAL_UART_Transmit(&huart2, (uint8_t *)encoded_data, (uint16_t)len, 1000);
}

void pss_comms_task()
{
	uint8_t data[DATA_LEN] = {0};

	while (true)
	{
		/* Wait for some data to arrive */
		cps_receive(&pipe, data, PIPE_WAIT_BLOCK);

		uint32_t packet_len = 0;

		/* Get the packet len from the MID */
		switch (cps_get_mid(data))
		{
		case MSGBatteryStats_MID:
			packet_len = MSGBatteryStats_LEN;
			break;
		default:
			packet_len = 0;
			break;
		}

		/* If it was an MID we want to send, do so */
		if (packet_len)
		{
			pss_comms_send(data, packet_len);
		}
	}

	vTaskDelete(NULL);
}

void pss_comms_start()
{
	cps_subscribe(MSGBatteryStats_MID, MSGBatteryStats_LEN, &pipe);

	xTaskCreate(pss_comms_task, "PSS Comms", 1024, NULL, tskIDLE_PRIORITY, NULL);
}