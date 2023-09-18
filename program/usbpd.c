#include "usbpd.h"

#include "cmsis_os.h"
#include "i2c.h"

#include "stusb4500.h"
#include "logging.h"

#define LOG_TAG "USBPD"

void stusb_read_burst(uint8_t reg_addr, uint8_t *data, uint16_t length)
{
    uint8_t status = HAL_I2C_Mem_Read(&hi2c3, STUSB_I2C_ADDR, (uint16_t)reg_addr, I2C_MEMADD_SIZE_8BIT, data, length, 10000);

    if (status != HAL_OK)
    {
        __NOP();
    }
}

void stusb_write_burst(uint8_t reg_addr, uint8_t *data, uint16_t length)
{
    if (HAL_I2C_Mem_Write(&hi2c3, STUSB_I2C_ADDR, (uint16_t)reg_addr, I2C_MEMADD_SIZE_8BIT, data, length, 10) != HAL_OK)
    {
        __NOP();
    }
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
    uint8_t pdo_number = stusb_get_pdo_selected();
    bool nint = stusb_get_nint();
    bool attach = stusb_get_attach();
    bool pok2 = stusb_get_pok2();
    bool pok3 = stusb_get_pok3();
    log_info("%s PDO selected is = %u", "USBPD", pdo_number);
    log_info("%s Interrupt level is = %u", LOG_TAG, nint);
    log_info("%s Attach level is = %u", LOG_TAG, attach);
    log_info("%s POK2 level is = %u", LOG_TAG, pok2);
    log_info("%s POK3 level is = %u", LOG_TAG, pok3);

    __NOP();
}
