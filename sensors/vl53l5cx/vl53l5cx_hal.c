/**
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#include "vl53l5cx_hal.h"
#include "../../sensor_app_i.h"
#include <furi_hal_i2c.h>
#include <core/kernel.h>
#include <core/core_defines.h>

uint8_t RdByte(VL53L5CX_Platform* p_platform, uint16_t RegisterAdress, uint8_t* p_value) {
    furi_hal_i2c_acquire(I2C_BUS);
    uint8_t status = !furi_hal_i2c_read_reg_8(
        I2C_BUS, p_platform->address, RegisterAdress, p_value, I2C_TIMEOUT);
    furi_hal_i2c_release(I2C_BUS);

    return status;
}

uint8_t WrByte(VL53L5CX_Platform* p_platform, uint16_t RegisterAdress, uint8_t value) {
    furi_hal_i2c_acquire(I2C_BUS);
    uint8_t status = !furi_hal_i2c_write_reg_8(
        I2C_BUS, p_platform->address, RegisterAdress, value, I2C_TIMEOUT);
    furi_hal_i2c_release(I2C_BUS);

    return status;
}

uint8_t WrMulti(
    VL53L5CX_Platform* p_platform,
    uint16_t RegisterAdress,
    uint8_t* p_values,
    uint32_t size) {
    furi_hal_i2c_acquire(I2C_BUS);
    uint8_t status = !furi_hal_i2c_write_mem(
        I2C_BUS, p_platform->address, RegisterAdress, p_values, size, I2C_TIMEOUT_LONG);
    furi_hal_i2c_release(I2C_BUS);

    return status;
}

uint8_t RdMulti(
    VL53L5CX_Platform* p_platform,
    uint16_t RegisterAdress,
    uint8_t* p_values,
    uint32_t size) {
    furi_hal_i2c_acquire(I2C_BUS);
    uint8_t status = !furi_hal_i2c_read_mem(
        I2C_BUS, p_platform->address, RegisterAdress, p_values, size, I2C_TIMEOUT_LONG);
    furi_hal_i2c_release(I2C_BUS);

    return status;
}

uint8_t Reset_Sensor(VL53L5CX_Platform* p_platform) {
    uint8_t status = 0;

    /* (Optional) Need to be implemented by customer. This function returns 0 if OK */

    /* Set pin LPN to LOW */
    /* Set pin AVDD to LOW */
    /* Set pin VDDIO  to LOW */
    WaitMs(p_platform, 100);

    /* Set pin LPN of to HIGH */
    /* Set pin AVDD of to HIGH */
    /* Set pin VDDIO of  to HIGH */
    WaitMs(p_platform, 100);

    return status;
}

void SwapBuffer(uint8_t* buffer, uint16_t size) {
    uint32_t i, tmp;

    /* Example of possible implementation using <string.h> */
    for(i = 0; i < size; i = i + 4) {
        tmp = (buffer[i] << 24) | (buffer[i + 1] << 16) | (buffer[i + 2] << 8) | (buffer[i + 3]);

        memcpy(&(buffer[i]), &tmp, 4);
    }
}

uint8_t WaitMs(VL53L5CX_Platform* p_platform, uint32_t TimeMs) {
    UNUSED(p_platform);
    furi_delay_ms(TimeMs);
    return 0;
}
