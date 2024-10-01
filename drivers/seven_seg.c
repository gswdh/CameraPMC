#include "seven_seg.h"

#include "tca6408.h"

#include "i2c.h"

static const uint8_t ss_numbers[10] = {
	(SS_SEG_A | SS_SEG_B | SS_SEG_C | SS_SEG_D | SS_SEG_E | SS_SEG_F),			  // 0
	(SS_SEG_B | SS_SEG_C),														  // 1
	(SS_SEG_A | SS_SEG_B | SS_SEG_D | SS_SEG_E | SS_SEG_G),						  // 2
	(SS_SEG_A | SS_SEG_B | SS_SEG_C | SS_SEG_D | SS_SEG_G),						  // 3
	(SS_SEG_B | SS_SEG_C | SS_SEG_F | SS_SEG_G),								  // 4
	(SS_SEG_A | SS_SEG_C | SS_SEG_D | SS_SEG_F | SS_SEG_G),						  // 5
	(SS_SEG_A | SS_SEG_C | SS_SEG_D | SS_SEG_E | SS_SEG_F | SS_SEG_G),			  // 6
	(SS_SEG_A | SS_SEG_B | SS_SEG_C),											  // 7
	(SS_SEG_A | SS_SEG_B | SS_SEG_C | SS_SEG_D | SS_SEG_E | SS_SEG_F | SS_SEG_G), // 8
	(SS_SEG_A | SS_SEG_B | SS_SEG_C | SS_SEG_D | SS_SEG_F | SS_SEG_G),			  // 9
};

void tca_i2c_write(uint8_t reg, uint8_t *data, uint32_t len)
{
	HAL_I2C_Mem_Write(&hi2c1, TCA_I2C_ADDR, (uint16_t)reg, I2C_MEMADD_SIZE_8BIT, data, (uint16_t)len, 15);
}

void tca_i2c_read(uint8_t reg, uint8_t *data, uint32_t len)
{
	HAL_I2C_Mem_Read(&hi2c1, TCA_I2C_ADDR, (uint16_t)reg, I2C_MEMADD_SIZE_8BIT, data, (uint16_t)len, 15);
}

void ss_set_segments(uint8_t segments)
{
	// 0x00 is all outputs
	tca_set_dir(0x00);

	// Set the segments
	tca_write_port(segments);
}

void ss_set_number(uint8_t n)
{
	if (n > 9)
	{
		return;
	}

	ss_set_segments(ss_numbers[n]);
}
