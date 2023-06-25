
#include "utils.h"
#include "../../sensor_app_i.h"
#include <stdint.h>
#include <stdbool.h>

bool utils_isI2CDeviceReady(uint8_t addr) {
    furi_hal_i2c_acquire(I2C_BUS);
    bool isReady = false;
    if(furi_hal_i2c_is_device_ready(I2C_BUS, addr, I2C_TIMEOUT)) isReady = true;
    furi_hal_i2c_release(I2C_BUS);
    return isReady;
}