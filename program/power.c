#include "power.h"

#include "program.h"
#include "gpio.h"
#include "i2c.h"
#include "adc.h"
#include "cmsis_os.h"
#include "usbpd.h"
#include "logging.h"
#include "act2861.h"
#include "charger.h"
#include "stusb4500.h"

#define LOG_TAG "PWR"

uint16_t pwr_measure_results[2] = {0};

act_error act_write_regs(uint8_t addr, uint8_t *data, uint8_t len)
{
	HAL_I2C_Mem_Write(&hi2c2, ACT_I2C_ADDR, (uint16_t)addr, I2C_MEMADD_SIZE_8BIT, data, (uint16_t)len, 15);
	return ACT_OK;
}

act_error act_read_regs(uint8_t addr, uint8_t *data, uint8_t len)
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
	osDelay(time_ms);
}

uint32_t act_get_tick_ms()
{
	return osKernelGetTickCount();
}

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

pwr_wake_source_t pwr_get_wake_source()
{
	if (stusb_get_attach() == 0)
	{
		return PWR_WAKE_USB;
	}

	return PWR_WAKE_BUTTON;
}

void pwr_start_charging(float max_input_current)
{
}

void pwr_stop_charging()
{
}

void pwr_start()
{
	usbpd_start();

	pwr_measure_start();

	xTaskCreate(pwr_task, "Power", 1024, NULL, tskIDLE_PRIORITY, NULL);
	xTaskCreate(chrg_task, "Charger", 1024, NULL, tskIDLE_PRIORITY, NULL);
}

void pwr_task(void *params)
{
	uint32_t tick = sys_get_tick();

	while (1)
	{
		if (sys_get_tick() > (tick + 1000))
		{
			tick = sys_get_tick();

			float v = pwr_measure_voltage_V();
			float a = pwr_measure_current_A();
			float w = pwr_measure_power_W();

			log_set_bar("System Voltage", v);
			log_set_bar("System Current", a);
			log_set_bar("System Power", w);
		}
	}
}

void chrg_task(void *params)
{
	bool usb_attached_n = true;

	CHRG_EnterHiZ();

	uint32_t tick = sys_get_tick();

	while (1)
	{
		if (sys_get_tick() > (tick + 1000))
		{
			tick = sys_get_tick();

			CHRG_ADCResults results = {0};
			CHRG_GetADCResults(&results);

			log_set_bar("Battery Voltage", results.v_bat_volts);
			log_set_bar("Battery Current", results.i_bat_amps);
			log_set_bar("Charger Current", results.i_in_amps);

			// Check to see if the USB has been plugged in or out
			if (usb_attached_n != stusb_get_attach())
			{
				// Update the current status
				usb_attached_n = stusb_get_attach();

				// If not attached, put charger into idle
				if (usb_attached_n)
				{
					CHRG_EnterHiZ();
					log_info(LOG_TAG, "USB has been detached, stopped charging.\n");
				}

				// If attached, attempt to start charging
				else
				{
					pdo_t pdo = stusb_read_pdo_selected();
					log_info(LOG_TAG, "PDO voltage = %2.3f, current = %2.3f.\n", pdo.voltage, pdo.current);
					act_error error = CHRG_EnableCharging(5, pdo.current);
					if (error == ACT_OK)
					{
						log_info(LOG_TAG, "USB has been attached. Started charging with a %2.3fA input current limit.\n", pdo.current);
					}

					else
					{
						log_info(LOG_TAG, "USB has been attached. Start charging failed with code = %u. Moving to idle.\n", error);
						CHRG_EnterHiZ();
					}
				}
			}
		}
	}
}
