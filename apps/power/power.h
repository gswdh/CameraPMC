#ifndef __PWR__
#define __PWR__

#include "main.h"

#include <stdbool.h>

typedef enum
{
	PWR_WAKE_UNKNOWN,
	PWR_WAKE_BUTTON,
	PWR_WAKE_USB,
	PWR_WAKE_BMS,
} pwr_wake_source_t;

#define PWR_MAX_CHARGE_CURRENT_A (3.5f)

void pwr_measure_start();
float pwr_measure_voltage_V();
float pwr_measure_current_A();
float pwr_measure_power_W();
float pwr_measure_power_T();

void pwr_sys_on();
void pwr_sys_off();
void pwr_sleep();
pwr_wake_source_t pwr_get_wake_source();

bool pwr_has_battery();

void pwr_start();
void pwr_task(void *params);

void pwr_chrg_start(void);
void pwr_usbpd_start(void);
void pwr_bms_start(void);

#endif