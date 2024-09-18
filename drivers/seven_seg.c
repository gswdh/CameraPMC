#include "seven_seg.h"

#include "tca6408.h"

#include "i2c.h"

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
