#include "powermgt.h"

#include "gpio.h"
#include "cmsis_os.h"

void pwr_sys_on()
{
	// General system power (5V)
	HAL_GPIO_WritePin(PWR_VBAT_EN_GPIO_Port, PWR_VBAT_EN_Pin, 1);
	osDelay(50);

	// FPGA and peripherals power up
	HAL_GPIO_WritePin(PWR_FPGA_1V0_EN_GPIO_Port, PWR_FPGA_1V0_EN_Pin, 1);
	HAL_GPIO_WritePin(PWR_ANA_1V8_EN_GPIO_Port, PWR_ANA_1V8_EN_Pin, 1);
	osDelay(10);
	HAL_GPIO_WritePin(PWR_DIG_1V8_EN_GPIO_Port, PWR_DIG_1V8_EN_Pin, 1);
	osDelay(10);
	HAL_GPIO_WritePin(PWR_DDR3_1V35_EN_GPIO_Port, PWR_DDR3_1V35_EN_Pin, 1);
	HAL_GPIO_WritePin(PWR_SYS_3V3_EN_GPIO_Port, PWR_SYS_3V3_EN_Pin, 1);
	osDelay(10);
}

void pwr_sys_off()
{
	// FPGA and peripherals power down
	HAL_GPIO_WritePin(PWR_DDR3_1V35_EN_GPIO_Port, PWR_DDR3_1V35_EN_Pin, 1);
	HAL_GPIO_WritePin(PWR_SYS_3V3_EN_GPIO_Port, PWR_SYS_3V3_EN_Pin, 1);
	osDelay(10);
	HAL_GPIO_WritePin(PWR_DIG_1V8_EN_GPIO_Port, PWR_DIG_1V8_EN_Pin, 1);
	osDelay(10);
	HAL_GPIO_WritePin(PWR_FPGA_1V0_EN_GPIO_Port, PWR_FPGA_1V0_EN_Pin, 1);
	HAL_GPIO_WritePin(PWR_ANA_1V8_EN_GPIO_Port, PWR_ANA_1V8_EN_Pin, 1);
	osDelay(10);

	// General system power (5V)
	osDelay(50);
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
