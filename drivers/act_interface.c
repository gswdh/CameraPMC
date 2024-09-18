#include "act2861.h"

#include "gpio.h"
#include "i2c.h"

#include "FreeRTOS.h"
#include "task.h"

act_error_t act_write_regs(uint8_t addr, uint8_t *data, uint8_t len)
{
	HAL_I2C_Mem_Write(&hi2c2, ACT_I2C_ADDR, (uint16_t)addr, I2C_MEMADD_SIZE_8BIT, data, (uint16_t)len, 15);
	return ACT_OK;
}

act_error_t act_read_regs(uint8_t addr, uint8_t *data, uint8_t len)
{
	HAL_I2C_Mem_Read(&hi2c2, ACT_I2C_ADDR, (uint16_t)addr, I2C_MEMADD_SIZE_8BIT, data, (uint16_t)len, 15);
	return ACT_OK;
}

bool act_get_irq_pin()
{
	return (bool)HAL_GPIO_ReadPin(ACT_NINT_GPIO_Port, ACT_NINT_Pin);
}

void act_delay_ms(uint32_t time_ms)
{
}

uint32_t act_get_tick_ms()
{
	return xTaskGetTickCount();
}
