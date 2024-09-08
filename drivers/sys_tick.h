#pragma once

#define SYS_DLY_MS(n) (vTaskDelay(n / portTICK_PERIOD_MS))
#define SYS_TCK_MS xTaskGetTickCount
