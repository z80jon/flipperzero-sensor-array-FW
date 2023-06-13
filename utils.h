#pragma once

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Checks the I2C bus to see if a device with the specified address responds when queried
 * 
 * @param addr the address of the device to check for
 * @return true the device was found
 * @return false the device was not found
 */
bool utils_isI2CDeviceReady(uint8_t addr);
