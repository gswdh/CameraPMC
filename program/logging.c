#include "logging.h"

#include <string.h>

#include "cmsis_os.h"
#include "usart.h"

uint8_t log_buffer[LOG_BUFFER_LEN] = {0};

void log_send_data(const char * msg, uint32_t msg_len)
{
    HAL_UART_Transmit(&huart1, (uint8_t*)msg, msg_len, 100);
}

uint32_t log_get_time()
{
    return osKernelGetTickCount();
}

void log_start()
{
	log_create_bar("Battery Voltage", "V", 8, 13);
	log_create_bar("Battery Current", "A", -5, 5);
	log_create_bar("Charger Current", "A", 0, 3.5);
	log_create_bar("System Voltage", "V", 4, 22);
	log_create_bar("System Current", "A", 0, 1);
	log_create_bar("System Power", "W", 0, 5);
}