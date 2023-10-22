#ifndef __PWR__
#define __PWR__

#include "main.h"

void pwr_measure_start();
float pwr_measure_voltage_V();
float pwr_measure_current_A();
float pwr_measure_power_W();
 
void pwr_sys_on();
void pwr_sys_off();
void pwr_sleep();

void pwr_start();
void pwr_task(void * params);
void chrg_task(void * params);

#endif