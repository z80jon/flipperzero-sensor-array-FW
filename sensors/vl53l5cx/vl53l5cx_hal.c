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

uint8_t RdByte(VL53L5CX_Platform* p_platform, uint16_t RegisterAdress, uint8_t* p_value) {
    uint8_t status = 255;

    /* Need to be implemented by customer. This function returns 0 if OK */

    return status;
}

uint8_t WrByte(VL53L5CX_Platform* p_platform, uint16_t RegisterAdress, uint8_t value) {
    uint8_t status = 255;

    /* Need to be implemented by customer. This function returns 0 if OK */

    return status;
}

uint8_t WrMulti(
    VL53L5CX_Platform* p_platform,
    uint16_t RegisterAdress,
    uint8_t* p_values,
    uint32_t size) {
    uint8_t status = 255;

    /* Need to be implemented by customer. This function returns 0 if OK */

    return status;
}

uint8_t RdMulti(
    VL53L5CX_Platform* p_platform,
    uint16_t RegisterAdress,
    uint8_t* p_values,
    uint32_t size) {
    uint8_t status = 255;

    /* Need to be implemented by customer. This function returns 0 if OK */

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
    uint8_t status = 255;

    /* Need to be implemented by customer. This function returns 0 if OK */

    return status;
}
