#include "usbpd.h"

#include "i2c.h"

#include "stusb4500.h"

void stusb_read_burst(uint8_t reg_addr, uint8_t *data, uint16_t length)
{
    if (HAL_I2C_Mem_Read(&hi2c3, STUSB_I2C_ADDR, (uint16_t)reg_addr, I2C_MEMADD_SIZE_8BIT, data, length, 10) != HAL_OK)
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

}

