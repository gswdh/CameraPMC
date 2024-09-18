#include "power.h"

#include "gpio.h"
#include "i2c.h"
#include "adc.h"
#include "stusb4500.h"

#include "sys_tick.h"

#include "FreeRTOS.h"
#include "task.h"

#include <stdint.h>

static uint16_t pwr_measure_results[3] = {0};

void pwr_measure_start()
{
	HAL_ADC_Start_DMA(&hadc1, (uint32_t *)pwr_measure_results, 3);
}

float pwr_measure_voltage_V()
{
	return (float)pwr_measure_results[1] * 6.591796875e-3;
}

float pwr_measure_current_A()
{
	return (float)pwr_measure_results[0] * 329.589844e-6;
}

float pwr_measure_power_W()
{
	return pwr_measure_current_A() * pwr_measure_voltage_V();
}

float pwr_measure_power_T()
{
	return __HAL_ADC_CALC_TEMPERATURE(1800, pwr_measure_results[2], ADC_RESOLUTION_12B);
}

void pwr_sys_on()
{
	// General system power (5V)
	HAL_GPIO_WritePin(PWR_VBAT_EN_GPIO_Port, PWR_VBAT_EN_Pin, 1);
	SYS_DLY_MS(50);

	// FPGA and peripherals power up
	HAL_GPIO_WritePin(PWR_FPGA_1V0_EN_GPIO_Port, PWR_FPGA_1V0_EN_Pin, 1);
	HAL_GPIO_WritePin(PWR_ANA_1V8_EN_GPIO_Port, PWR_ANA_1V8_EN_Pin, 1);
	SYS_DLY_MS(10);
	HAL_GPIO_WritePin(PWR_DIG_1V8_EN_GPIO_Port, PWR_DIG_1V8_EN_Pin, 1);
	SYS_DLY_MS(10);
	HAL_GPIO_WritePin(PWR_DDR3_1V35_EN_GPIO_Port, PWR_DDR3_1V35_EN_Pin, 1);
	HAL_GPIO_WritePin(PWR_SYS_3V3_EN_GPIO_Port, PWR_SYS_3V3_EN_Pin, 1);
	SYS_DLY_MS(10);

	HAL_GPIO_WritePin(FPGA_NINIT_GPIO_Port, FPGA_NINIT_Pin, 1);
	HAL_GPIO_WritePin(FPGA_NPROG_GPIO_Port, FPGA_NPROG_Pin, 1);
}

void pwr_sys_off()
{
	// FPGA and peripherals power down
	HAL_GPIO_WritePin(PWR_DDR3_1V35_EN_GPIO_Port, PWR_DDR3_1V35_EN_Pin, 1);
	HAL_GPIO_WritePin(PWR_SYS_3V3_EN_GPIO_Port, PWR_SYS_3V3_EN_Pin, 1);
	SYS_DLY_MS(10);
	HAL_GPIO_WritePin(PWR_DIG_1V8_EN_GPIO_Port, PWR_DIG_1V8_EN_Pin, 1);
	SYS_DLY_MS(10);
	HAL_GPIO_WritePin(PWR_FPGA_1V0_EN_GPIO_Port, PWR_FPGA_1V0_EN_Pin, 1);
	HAL_GPIO_WritePin(PWR_ANA_1V8_EN_GPIO_Port, PWR_ANA_1V8_EN_Pin, 1);
	SYS_DLY_MS(10);

	// General system power (5V)
	SYS_DLY_MS(50);
	HAL_GPIO_WritePin(PWR_VBAT_EN_GPIO_Port, PWR_VBAT_EN_Pin, 1);
}

void pwr_sleep()
{
	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);

	__HAL_RCC_PWR_CLK_ENABLE();

	// Stop while clearing the flags
	HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);

	// Clear all wake up flags
	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);

	// Enable WKUP pin
	HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);

	// Simply enter the standby mode!
	HAL_PWR_EnterSTANDBYMode();
}

pwr_wake_source_t pwr_get_wake_source()
{
	if (stusb_get_attach() == 0)
	{
		return PWR_WAKE_USB;
	}

	return PWR_WAKE_BUTTON;
}

bool pwr_has_battery()
{
	return !HAL_GPIO_ReadPin(BMS_NPRESENT_GPIO_Port, BMS_NPRESENT_Pin);
}
