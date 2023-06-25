#include "amg8833.h"
#include "../../sensor_app_i.h"
#include "../utils/utils.h"
#include <furi_hal_i2c.h>
#include <furi_hal_i2c_config.h>
#include <unistd.h>
#include <stdlib.h>
#include <core/check.h>
#include <furi.h>
#include <furi_hal.h>
#include <math.h>

//Registers
#define REGISTER_POWER_CONTROL 0x00 //< Register to set the amg8833 to wake/sleep
#define REGISTER_FRAMERATE 0x02 //< Register to set framerate to 1 vs 10fps
#define REGISTER_STATUS 0x04 //< Shows current status of AMG8833
#define REGISTER_TEMPERATURE_BASE_ADDR \
    0x80 //< Base address of the 128 temperature registers for the IR imager

//Power settings
#define POWER__NORMAL_MODE 0x00
#define POWER__SLEEP_MODE 0x10

AMG8833* AMG8833_init(uint8_t addr, eAMG8833Framerate frameRate) {
    //Make sure the module is there before we do anything
    furi_check(utils_isI2CDeviceReady(addr));

    //Struct setup
    AMG8833* sensor = (AMG8833*)malloc(sizeof(AMG8833));
    sensor->addr = addr;
    sensor->freshData = false;
    sensor->status = AMG8833Status_OK;

    //Make sure we're in normal mode
    furi_check(AMG8833_wake(sensor) == 0);

    //Set framerate to normal
    furi_check(AMG8833_setFrameRate(sensor, frameRate) == 0);

    return sensor;
}

eAMG8833Status AMG8833_getStatus(AMG8833* sensor) {
    furi_hal_i2c_acquire(I2C_BUS);
    uint8_t status = furi_hal_i2c_read_reg_8(
        I2C_BUS, sensor->addr, REGISTER_POWER_CONTROL, &status, I2C_TIMEOUT);
    furi_hal_i2c_release(I2C_BUS);

    switch(status) {
    case POWER__NORMAL_MODE:
        sensor->status = AMG8833Status_OK;
        break;

    case POWER__SLEEP_MODE:
        sensor->status = AMG8833Status_Sleep;
        break;

    default:
        sensor->status = AMG8833Status_Error;
    }

    return AMG8833Status_OK;
}

int AMG8833_setFrameRate(AMG8833* sensor, eAMG8833Framerate frameRate) {
    furi_hal_i2c_acquire(I2C_BUS);

    //Make sure the framerate is valid
    furi_assert(frameRate == AMG8833FrameRate_10FPS || frameRate == AMG8833FrameRate_1FPS);

    //The framerate enums equate to the actual register value to write, so just send them
    //If it doesn't go through, we fail the check.
    furi_assert(furi_hal_i2c_write_reg_8(
        I2C_BUS, sensor->addr, REGISTER_FRAMERATE, frameRate, I2C_TIMEOUT));

    //Update the data structure
    sensor->frameRate = frameRate;

    furi_hal_i2c_release(I2C_BUS);
    return 0;
}

int AMG8833_sleep(AMG8833* sensor) {
    furi_hal_i2c_acquire(I2C_BUS);

    furi_check(furi_hal_i2c_write_reg_8(
        I2C_BUS, sensor->addr, REGISTER_POWER_CONTROL, POWER__SLEEP_MODE, I2C_TIMEOUT));

    sensor->status = AMG8833Status_Sleep;

    furi_hal_i2c_release(I2C_BUS);
    return 0;
}

int AMG8833_wake(AMG8833* sensor) {
    furi_hal_i2c_acquire(I2C_BUS);

    furi_check(furi_hal_i2c_write_reg_8(
        I2C_BUS, sensor->addr, REGISTER_POWER_CONTROL, POWER__NORMAL_MODE, I2C_TIMEOUT));

    sensor->status = AMG8833Status_OK;

    furi_hal_i2c_release(I2C_BUS);
    return 0;
}

int AMG8833_update(AMG8833* sensor) {
    furi_check(sensor != NULL);

    //Grab temperatures
    furi_hal_i2c_acquire(I2C_BUS);
    furi_check(furi_hal_i2c_read_mem(
        I2C_BUS, sensor->addr, REGISTER_TEMPERATURE_BASE_ADDR, sensor->tempData, 128, 100));
    furi_hal_i2c_release(I2C_BUS);

    //Update min and max
    sensor->max = AMG8833_getTemperature(sensor, 0);
    sensor->min = sensor->max;
    for(uint8_t i = 0; i < 64; i++) {
        float temperature = AMG8833_getTemperature(sensor, i);
        if(temperature > sensor->max) sensor->max = temperature;
        if(temperature < sensor->min) sensor->min = temperature;
    }

    //Create 9 'bins' we can split the data into and calculate where the 'separators' between them should be based on max vs min
    float binSeparators[NUM_GRAYSCALE_BINS];
    float interval = (sensor->max - sensor->min) / NUM_GRAYSCALE_BINS;
    for(uint8_t i = 0; i < NUM_GRAYSCALE_BINS; i++)
        binSeparators[i] = sensor->min + (interval * i);

    //Sort each of the 64 pixels into its respective bin
    for(uint8_t i = 0; i < 64; i++) {
        float temp = AMG8833_getTemperature(sensor, i);
        sensor->tempDataBins[i] = 0;
        for(uint8_t bin = 1; bin < NUM_GRAYSCALE_BINS; bin++) {
            if(temp > binSeparators[bin - 1]) {
                sensor->tempDataBins[i] = bin;
            }
        }
    }

    return 0;
}

float AMG8833_getTemperature(AMG8833* sensor, uint8_t pixelAddr) {
    furi_check(pixelAddr < 64);
    furi_check(sensor != NULL);
    //if(sensor->status != AMG8833Status_OK) return (float)-999;
    uint16_t temp_raw = ((sensor->tempData[pixelAddr * 2 + 1] & 0x07) << 8) |
                        sensor->tempData[pixelAddr * 2];

    float temperature = ((float)temp_raw) * 0.25;

    if((sensor->tempData[pixelAddr * 2 + 1] | 0x08) == 1) //Sign bit check
        temperature = -temperature;

    return temperature;
}

int16_t AMG8833_getTemperatureInt(AMG8833* sensor, uint8_t pixelAddr) {
    return (int16_t)AMG8833_getTemperature(sensor, pixelAddr);
}

uint8_t AMG8833_getTemperatureGrayscale(AMG8833* sensor, uint8_t pixelAddr) {
    furi_check(sensor != NULL);
    furi_check(pixelAddr < 64);
    return sensor->tempDataBins[pixelAddr];
}

int AMG8833_free(AMG8833* sensor) {
    furi_assert(sensor != NULL);
    AMG8833_sleep(sensor);
    free(sensor);
    return 0;
}
