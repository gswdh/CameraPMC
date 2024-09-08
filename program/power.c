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

#include "cpubsub.h"
#include "messages.h"

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#define LOG_TAG "PWR"

static uint16_t pwr_measure_results[3] = {0};
static MSGBatteryStats_t bat_stats_msg = {0};
static bool usb_attached = false;

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

bool pwr_has_battery()
{
	return !HAL_GPIO_ReadPin(BMS_NPRESENT_GPIO_Port, BMS_NPRESENT_Pin);
}

static void pwr_tick(TimerHandle_t timer)
{
	float v = pwr_measure_voltage_V();
	float a = pwr_measure_current_A();
	float w = pwr_measure_power_W();
	float t = pwr_measure_power_T();

	log_info(LOG_TAG, "System voltage = %2.3fV\n", v);
	log_info(LOG_TAG, "System current = %2.3fA\n", a);
	log_info(LOG_TAG, "Power consumption = %2.3fW\n", w);
	log_info(LOG_TAG, "Temperature = %2.3fC\n", t);
}

static void chg_tick(TimerHandle_t timer)
{
	CHRG_ADCResults results = {0};
	CHRG_GetADCResults(&results);

	/* Publish the data */
	bat_stats_msg.mid = MSGBatteryStats_MID;
	bat_stats_msg.voltage = results.v_adc_volts;
	bat_stats_msg.current = results.i_bat_amps;
	bat_stats_msg.soc = 0;
	cps_publish(&bat_stats_msg);

	// Check to see if the USB has been plugged in or out
	if (usb_attached == stusb_get_attach())
	{
		// Nothing has changed
		return;
	}

	// Update the current status
	usb_attached = stusb_get_attach();

	// If not attached, put charger into idle
	if (usb_attached == false)
	{
		CHRG_EnterHiZ();
		log_info(LOG_TAG, "USB has been detached, stopped charging.\n");
		return;
	}

	// As we're attached, attempt to start charging
	log_info(LOG_TAG, "A USB power source has been connected.\n");

	// Check if there's a pack attached
	if (pwr_has_battery() == false)
	{
		log_info(LOG_TAG, "No battery pack detected, will not attempt to charge.\n");
		return;
	}

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

void pwr_start()
{
	usbpd_start();

	pwr_measure_start();

	CHRG_EnterHiZ();

	TimerHandle_t pwr_timer = xTimerCreate("Power Tick", pdMS_TO_TICKS(1000), true, NULL, pwr_tick);
	xTimerStart(pwr_timer, 0);

	TimerHandle_t chg_timer = xTimerCreate("Charger Tick", pdMS_TO_TICKS(1000), true, NULL, chg_tick);
	xTimerStart(chg_timer, 0);
}
