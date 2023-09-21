#include "logging.h"

#include <string.h>

#include "cmsis_os.h"
#include "usart.h"

uint8_t log_buffer[LOG_BUFFER_LEN] = {0};

void log_send_data(const char * msg, uint32_t msg_len)
{
    HAL_UART_Transmit(&huart1, msg, msg_len, 100);
}

uint32_t log_get_time()
{
    return osKernelGetTickCount();
}

void log_start()
{

}