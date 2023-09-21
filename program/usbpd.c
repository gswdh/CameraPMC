#include "usbpd.h"

#include "cmsis_os.h"
#include "i2c.h"

#include "stusb4500.h"
#include "logging.h"

#define LOG_TAG "USBPD"

void stusb_read_burst(uint8_t reg_addr, uint8_t *data, uint16_t length)
{
    HAL_I2C_Mem_Read(&hi2c3, STUSB_I2C_ADDR, (uint16_t)reg_addr, I2C_MEMADD_SIZE_8BIT, data, length, 10000);
}

void stusb_write_burst(uint8_t reg_addr, uint8_t *data, uint16_t length)
{
    HAL_I2C_Mem_Write(&hi2c3, STUSB_I2C_ADDR, (uint16_t)reg_addr, I2C_MEMADD_SIZE_8BIT, data, length, 10);
}

void stusb_set_reset(bool en)
{
    HAL_GPIO_WritePin(STUSB_RESET_GPIO_Port, STUSB_RESET_Pin, en);
}

bool stusb_get_attach()
{
    return HAL_GPIO_ReadPin(STUSB_ATTACH_GPIO_Port, STUSB_ATTACH_Pin);
}

bool stusb_get_nint()
{
    return HAL_GPIO_ReadPin(STUSB_NINT_GPIO_Port, STUSB_NINT_Pin);
}

bool stusb_get_pok2()
{
    return HAL_GPIO_ReadPin(STUSB_PD0_GPIO_Port, STUSB_PD0_Pin);
}

bool stusb_get_pok3()
{
    return HAL_GPIO_ReadPin(STUSB_PD1_GPIO_Port, STUSB_PD1_Pin);
}

void usbpd_start()
{
    for(uint8_t i = 1; i < 4; i++)
    {
        pdo_t pdo = stusb_read_pdo(i);
        __NOP();
    }

    pdo_select_t pdo_number = stusb_get_pdo();
    stusb_set_pdo(PDO2);
    pdo_number = stusb_get_pdo();
    
    bool nint = stusb_get_nint();
    bool attach = stusb_get_attach();
    bool pok2 = stusb_get_pok2();
    bool pok3 = stusb_get_pok3();
    log_info(LOG_TAG, "PDO selected is = %u\n", pdo_number);
    log_info(LOG_TAG, "Interrupt level is = %u\n", nint);
    log_info(LOG_TAG, "Attach level is = %u\n", attach);
    log_info(LOG_TAG, "POK2 level is = %u\n", pok2);
    log_info(LOG_TAG, "POK3 level is = %u\n", pok3);

    __NOP();
}
