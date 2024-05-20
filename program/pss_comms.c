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

static uint32_t pss_comms_encode_data(uint8_t *data, uint32_t len, char *encoded_data)
{
	/* Calc the encoded data length */
	uint32_t encoded_data_len = BASE64_ENCODE_OUT_SIZE(len);

	/* Provision some memory */
	encoded_data = malloc(encoded_data_len + 1);
	assert(encoded_data != NULL);

	/* Encode and add a terminator */
	base64_encode(data, len, encoded_data);
	encoded_data[encoded_data_len] = '\n';

	/* Return the length for TX */
	return encoded_data_len;
}

void pss_comms_task()
{
	uint8_t data[DATA_LEN] = {0};

	while (true)
	{
		/* Wait for some data to arrive */
		cps_receive(&pipe, data, PIPE_WAIT_BLOCK);

		topic_t mid = cps_get_mid(data);

		if (mid == MSGBatteryStats_MID)
		{
			char *encoded_data = NULL;
			uint32_t len = pss_comms_encode_data(data, MSGBatteryStats_LEN, encoded_data);
			HAL_UART_Transmit(&huart2, (uint8_t *)encoded_data, (uint16_t)len, 1000);
			free(encoded_data);
		}
	}

	vTaskDelete(NULL);
}

void pss_comms_start()
{
	cps_subscribe(MSGBatteryStats_MID, MSGBatteryStats_LEN, &pipe);

	xTaskCreate(pss_comms_task, "PSS Comms", 1024, NULL, tskIDLE_PRIORITY, NULL);
}