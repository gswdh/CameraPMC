#include "logging.h"

#include <string.h>

#include "cmsis_os.h"
#include "usart.h"

uint8_t log_buffer[LOG_BUFFER_LEN] = {0};

static const char *level_strings[] = {
    "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};

static void print_callback(log_Event *ev)
{
    memset(log_buffer, 0, LOG_BUFFER_LEN);

    char params[256] = {0};
    sprintf(params, ev->fmt, ev->ap);

    sprintf(
        ev->udata, "%lu %s: %s\n",
        osKernelSysTick(), level_strings[ev->level], params);

    HAL_UART_Transmit_DMA(&huart1, log_buffer, strlen((char*)log_buffer));
}

void log_start()
{
    // Add the local callback to process log events and print them onto the UART
    log_add_callback(print_callback, (void *)log_buffer, 0);
}