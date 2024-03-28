#ifndef __PWR__
#define __PWR__

#include "main.h"

#include <stdbool.h>

typedef enum {PWR_WAKE_UNKNOWN, PWR_WAKE_BUTTON, PWR_WAKE_USB} pwr_wake_source_t;

void pwr_measure_start();
float pwr_measure_voltage_V();
float pwr_measure_current_A();
float pwr_measure_power_W();
 
void pwr_sys_on();
void pwr_sys_off();
void pwr_sleep();
pwr_wake_source_t pwr_get_wake_source();

void pwr_start_charging(float max_input_current);
void pwr_stop_charging();
bool pwr_has_battery();

void pwr_start();
void pwr_task(void * params);
void chrg_task(void * params);

#endif