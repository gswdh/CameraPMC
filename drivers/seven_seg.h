#pragma once

#include <stdint.h>

#define SS_SEG_A (0x01 << 2)
#define SS_SEG_B (0x01 << 3)
#define SS_SEG_C (0x01 << 6)
#define SS_SEG_D (0x01 << 5)
#define SS_SEG_E (0x01 << 4)
#define SS_SEG_F (0x01 << 1)
#define SS_SEG_G (0x01 << 0)
#define SS_SEG_P (0x01 << 7)

void ss_set_segments(uint8_t segments);
void ss_set_number(uint8_t n);