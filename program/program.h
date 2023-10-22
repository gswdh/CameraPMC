#ifndef __PRGRM_H__
#define __PRGRM_H__

#include <stdint.h>

void program_entry();

void sys_delay(uint32_t t_delay_ms);
uint32_t sys_get_tick();

#endif