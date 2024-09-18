#ifndef __SYS_H__
#define __SYS_H__

#include <stdint.h>
#include <stdbool.h>

#define T_OFF_BUTTON_TO_MS (2000)

extern bool is_charging;

void button_start(void);
void blink_start(void);

void system_start();

#endif