#include "power.h"

#include "gpio.h"
#include "adc.h"
#include "cmsis_os.h"
#include "usbpd.h"
#include "logging.h"

#define LOG_TAG "PWR"

osThreadId_t power_task_handle;
const osThreadAttr_t power_task_attributes = {
	.name = "power_task",
	.stack_size = 1024,
	.priority = (osPriority_t)osPriorityNormal,
};

uint16_t pwr_measure_results[2] = {0};

void pwr_measure_start()
{
	HAL_ADC_Start_DMA(&hadc1, (uint32_t *)pwr_measure_results, 2);
}

float pwr_measure_voltage_V()
{
	return (float)pwr_measure_results[1] * 6.591796875e-3;
}

float pwr_measure_current_A()
{
	return (float)pwr_measure_results[0] * 263.671875e-6;
}

float pwr_measure_power_W()
{
	return pwr_measure_current_A() * pwr_measure_voltage_V();
}

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

	HAL_GPIO_WritePin(FPGA_NINIT_GPIO_Port, FPGA_NINIT_Pin, 1);
	HAL_GPIO_WritePin(FPGA_NPROG_GPIO_Port, FPGA_NPROG_Pin, 1);
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

void pwr_start()
{
	usbpd_start();

	pwr_sys_on();

	pwr_measure_start();

	power_task_handle = osThreadNew(pwr_task, NULL, &power_task_attributes);
}

void pwr_task(void *params)
{
	uint32_t tick = osKernelGetTickCount();

	while (1)
	{
		if (osKernelGetTickCount() > (tick + 1000))
		{
			tick = osKernelGetTickCount();

			float v = pwr_measure_voltage_V();
			float a = pwr_measure_current_A();
			float w = pwr_measure_power_W();
			
			log_info(LOG_TAG, "System consumption = %2.3fV %2.3fA %2.3fW\n", v, a, w);
		}
	}
}
