/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "power.h"
#include "logging.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define LOG_TAG "INIT"
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t startup_task_handle;
osThreadId_t blink_task_handle;
osThreadId_t button_task_handle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void startup_task(void *args);
void blink_task(void *args);
void button_task(void *args);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
  log_start();
  log_info(LOG_TAG, "Starting system\n");

  pwr_start();
  pwr_sys_on();

  startup_task_handle = osThreadNew(startup_task, NULL, &defaultTask_attributes);
}

void startup_task(void *args)
{
  blink_task_handle = osThreadNew(blink_task, NULL, &defaultTask_attributes);
  button_task_handle = osThreadNew(button_task, NULL, &defaultTask_attributes);

  osThreadTerminate(osThreadGetId());
}

void button_task(void *args)
{
  while(HAL_GPIO_ReadPin(SYS_WAKE_GPIO_Port, SYS_WAKE_Pin))
  {
    osDelay(1);
  }

  while(1)
  {
    osDelay(1);

    uint32_t tick = osKernelSysTick();

    while(HAL_GPIO_ReadPin(SYS_WAKE_GPIO_Port, SYS_WAKE_Pin))
    {
      if(osKernelSysTick() > (tick + 1000))
      {
        log_info(LOG_TAG, "Button (thumb) held for more than one second, turning system off.");

        while(HAL_GPIO_ReadPin(SYS_WAKE_GPIO_Port, SYS_WAKE_Pin))
        {
          osDelay(100);
        }

        pwr_sys_off();

        pwr_sleep();       
      }
    }
  }
}

void blink_task(void *args)
{
  while(1)
  {
    HAL_GPIO_TogglePin(PMC_LED_RUN_GPIO_Port, PMC_LED_RUN_Pin);
    osDelay(50);
  }
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

