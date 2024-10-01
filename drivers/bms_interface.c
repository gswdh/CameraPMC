#include "max17320.h"

#include "i2c.h"
#include "gpio.h"

#include "logging.h"

#include <stdbool.h>

bms_error_t bms_reg_write(uint16_t reg, uint8_t *data, uint16_t len)
{
	if (HAL_I2C_Mem_Write(&hi2c1, BMS_I2C_ADDR(reg), (uint16_t)(reg & 0xFF), I2C_MEMADD_SIZE_8BIT, data, (uint16_t)len, 15) != HAL_OK)
	{
		return BMS_I2C_MEM_WRITE_ERROR;
	}

	return BMS_OK;
}

bms_error_t bms_reg_read(uint16_t reg, uint8_t *data, uint16_t len)
{
	if (HAL_I2C_Mem_Read(&hi2c1, BMS_I2C_ADDR(reg), (uint16_t)(reg & 0xFF), I2C_MEMADD_SIZE_8BIT, data, (uint16_t)len, 15) != HAL_OK)
	{
		return BMS_I2C_MEM_READ_ERROR;
	}

	return BMS_OK;
}
