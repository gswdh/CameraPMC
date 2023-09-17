/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define MON_SYS_BUS_A_Pin GPIO_PIN_0
#define MON_SYS_BUS_A_GPIO_Port GPIOC
#define MON_SYS_BUS_V_Pin GPIO_PIN_1
#define MON_SYS_BUS_V_GPIO_Port GPIOC
#define SYS_WAKE_Pin GPIO_PIN_0
#define SYS_WAKE_GPIO_Port GPIOA
#define PWR_VBAT_EN_Pin GPIO_PIN_1
#define PWR_VBAT_EN_GPIO_Port GPIOA
#define PSS_PMC_UART_RX_Pin GPIO_PIN_2
#define PSS_PMC_UART_RX_GPIO_Port GPIOA
#define PSS_PMC_UART_TX_Pin GPIO_PIN_3
#define PSS_PMC_UART_TX_GPIO_Port GPIOA
#define BMS_NPRESENT_Pin GPIO_PIN_4
#define BMS_NPRESENT_GPIO_Port GPIOA
#define BMS_NINT_Pin GPIO_PIN_5
#define BMS_NINT_GPIO_Port GPIOA
#define PSS_NSRST_Pin GPIO_PIN_6
#define PSS_NSRST_GPIO_Port GPIOA
#define STUSB_I2C_SCL_Pin GPIO_PIN_7
#define STUSB_I2C_SCL_GPIO_Port GPIOA
#define PSS_NRST_Pin GPIO_PIN_4
#define PSS_NRST_GPIO_Port GPIOC
#define PSS_PMC_FLAG_Pin GPIO_PIN_5
#define PSS_PMC_FLAG_GPIO_Port GPIOC
#define PMC_PSS_FLAG_Pin GPIO_PIN_0
#define PMC_PSS_FLAG_GPIO_Port GPIOB
#define ACT_NCHG_Pin GPIO_PIN_1
#define ACT_NCHG_GPIO_Port GPIOB
#define ACT_NINT_Pin GPIO_PIN_2
#define ACT_NINT_GPIO_Port GPIOB
#define ACT_I2C_SDA_Pin GPIO_PIN_10
#define ACT_I2C_SDA_GPIO_Port GPIOB
#define ACT_I2C_SCL_Pin GPIO_PIN_11
#define ACT_I2C_SCL_GPIO_Port GPIOB
#define FPGA_NPROG_Pin GPIO_PIN_12
#define FPGA_NPROG_GPIO_Port GPIOB
#define STUSB_PD0_Pin GPIO_PIN_14
#define STUSB_PD0_GPIO_Port GPIOB
#define STUSB_NINT_Pin GPIO_PIN_15
#define STUSB_NINT_GPIO_Port GPIOB
#define STUSB_ORI_Pin GPIO_PIN_6
#define STUSB_ORI_GPIO_Port GPIOC
#define STUSB_GPIO_Pin GPIO_PIN_7
#define STUSB_GPIO_GPIO_Port GPIOC
#define STUSB_PD1_Pin GPIO_PIN_8
#define STUSB_PD1_GPIO_Port GPIOC
#define STUSB_ATTACH_Pin GPIO_PIN_9
#define STUSB_ATTACH_GPIO_Port GPIOC
#define STUSB_RESET_Pin GPIO_PIN_8
#define STUSB_RESET_GPIO_Port GPIOA
#define PMC_UART_TX_Pin GPIO_PIN_9
#define PMC_UART_TX_GPIO_Port GPIOA
#define PMC_UART_RX_Pin GPIO_PIN_10
#define PMC_UART_RX_GPIO_Port GPIOA
#define PWR_SYS_3V3_EN_Pin GPIO_PIN_11
#define PWR_SYS_3V3_EN_GPIO_Port GPIOA
#define PWR_DIG_1V8_EN_Pin GPIO_PIN_12
#define PWR_DIG_1V8_EN_GPIO_Port GPIOA
#define PWR_DDR3_1V35_EN_Pin GPIO_PIN_15
#define PWR_DDR3_1V35_EN_GPIO_Port GPIOA
#define PMC_LED_RUN_Pin GPIO_PIN_10
#define PMC_LED_RUN_GPIO_Port GPIOC
#define PMC_LED_FPGA_OK_Pin GPIO_PIN_11
#define PMC_LED_FPGA_OK_GPIO_Port GPIOC
#define PMC_LED_STATUS_Pin GPIO_PIN_12
#define PMC_LED_STATUS_GPIO_Port GPIOC
#define PWR_FPGA_1V0_EN_Pin GPIO_PIN_3
#define PWR_FPGA_1V0_EN_GPIO_Port GPIOB
#define STUSB_I2C_SDA_Pin GPIO_PIN_4
#define STUSB_I2C_SDA_GPIO_Port GPIOB
#define PWR_ANA_1V8_EN_Pin GPIO_PIN_5
#define PWR_ANA_1V8_EN_GPIO_Port GPIOB
#define BMS_I2C_SCL_Pin GPIO_PIN_6
#define BMS_I2C_SCL_GPIO_Port GPIOB
#define BMS_I2C_SDA_Pin GPIO_PIN_7
#define BMS_I2C_SDA_GPIO_Port GPIOB
#define FPGA_NINIT_Pin GPIO_PIN_8
#define FPGA_NINIT_GPIO_Port GPIOB
#define FPGA_DONE_Pin GPIO_PIN_9
#define FPGA_DONE_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
