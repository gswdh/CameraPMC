#ifndef __PWR__
#define __PWR__

#include "main.h"

#include <stdbool.h>

typedef enum
{
	PWR_WAKE_UNKNOWN = 0,
	PWR_WAKE_BUTTON,
	PWR_WAKE_USB,
} pwr_wake_source_t;

typedef enum
{
	PWR_STATE_UNINIT = 0,
	PWR_STATE_OFF,
	PWR_STATE_CHARGING,
	PWR_STATE_RUN,
} pwr_state_t;

#define PWR_MAX_CHARGE_CURRENT_A (3.5f)

#define PWR_POWER_AVG_LEN (20)

void pwr_measure_start();
bool pwr_bms_nirq(void);
float pwr_measure_voltage_V();
float pwr_measure_current_A();
float pwr_measure_power_W();
float pwr_measure_power_T();

void pwr_sys_on();
void pwr_sys_off();
void pwr_sleep();
pwr_wake_source_t pwr_get_wake_source();

bool pwr_has_battery();
bool pwr_is_charging();

void pwr_start();
void pwr_task(void *params);

void pwr_chrg_start(void);
void pwr_usbpd_start(void);
void pwr_bms_start(void);

#endif