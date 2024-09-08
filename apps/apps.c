// Matching header
#include "apps.h"

#include "logging.h"

// App headers
#include "system.h"
#include "power.h"

// FreeRTOS
#include "FreeRTOS.h"
#include "task.h"

// System headers
#include <stdint.h>

#define LOG_TAG "APPS"

typedef struct
{
	TaskFunction_t function;
	const char name[32];
	const uint32_t stack_size;
} app_config_t;

static const app_config_t app_configs[] = {
	{
		.function = &system_start,
		.name = "System",
		.stack_size = 256,
	},
	{
		.function = &pwr_start,
		.name = "Power",
		.stack_size = 256,
	},
};

void apps_launcher(void)
{
	for (uint32_t i = 0; i < (sizeof(app_configs) / sizeof(app_configs[0])); i++)
	{
		log_info(LOG_TAG, "Start app %s\n", app_configs[i].name);
		xTaskCreate(app_configs[i].function, app_configs[i].name, app_configs[i].stack_size, NULL, tskIDLE_PRIORITY, NULL);
	}
}