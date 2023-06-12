#include "grideye.h"
#include "../sensor_app_i.h"
#include <furi_hal_i2c.h>
#include <furi_hal_i2c_config.h>
#include <unistd.h>
#include <stdlib.h>
#include <core/check.h>
#include <furi.h>
#include <furi_hal.h>
#include <math.h>

//Number of temperature 'bins' (for grayscale data generation)
#define NUM_TEMPERATURE_BINS 9

//Registers
#define REGISTER_POWER_CONTROL 0x00 //< Register to set the amg8833 to wake/sleep
#define REGISTER_FRAMERATE 0x02 //< Register to set framerate to 1 vs 10fps
#define REGISTER_STATUS 0x04 //< Shows current status of AMG8833
#define REGISTER_TEMPERATURE_BASE_ADDR \
    0x80 //< Base address of the 128 temperature registers for the IR imager

//Power settings
#define POWER__NORMAL_MODE 0x00
#define POWER__SLEEP_MODE 0x10

GridEye* gridEye_init(uint8_t addr, eGridEyeFramerate frameRate) {
    //Make sure the module is there before we do anything
    furi_hal_i2c_acquire(I2C_BUS);
    furi_check(furi_hal_i2c_is_device_ready(I2C_BUS, addr, I2C_TIMEOUT));
    furi_hal_i2c_release(I2C_BUS);

    //Struct setup
    GridEye* ge = (GridEye*)malloc(sizeof(GridEye));
    ge->addr = addr;
    ge->freshData = false;
    ge->status = GridEyeStatus_OK;

    //Make sure we're in normal mode
    furi_check(gridEye_wake(ge) == 0);

    //Set framerate to normal
    furi_check(gridEye_setFrameRate(ge, frameRate) == 0);

    //TODO setup defaults (add #define in .c), update status

    return ge;
}

eGridEyeStatus gridEye_getStatus(GridEye* ge) {
    furi_hal_i2c_acquire(I2C_BUS);
    furi_check(furi_hal_i2c_is_device_ready(I2C_BUS, ge->addr, I2C_TIMEOUT));

    uint8_t status =
        furi_hal_i2c_read_reg_8(I2C_BUS, ge->addr, REGISTER_POWER_CONTROL, &status, I2C_TIMEOUT);

    switch(status) {
    case POWER__NORMAL_MODE:
        ge->status = GridEyeStatus_OK;
        break;

    case POWER__SLEEP_MODE:
        ge->status = GridEyeStatus_Sleep;
        break;

    default:
        ge->status = GridEyeStatus_Error;
    }

    furi_hal_i2c_release(I2C_BUS);

    return GridEyeStatus_OK;
}

int gridEye_setFrameRate(GridEye* ge, eGridEyeFramerate frameRate) {
    furi_hal_i2c_acquire(I2C_BUS);

    //Make sure the framerate is valid
    furi_assert(frameRate == GridEyeFrameRate_10FPS || frameRate == GridEyeFrameRate_1FPS);

    //The framerate enums equate to the actual register value to write, so just send them
    //If it doesn't go through, we fail the check.
    furi_assert(
        furi_hal_i2c_write_reg_8(I2C_BUS, ge->addr, REGISTER_FRAMERATE, frameRate, I2C_TIMEOUT));

    //Update the data structure
    ge->frameRate = frameRate;

    furi_hal_i2c_release(I2C_BUS);
    return 0;
}

int gridEye_sleep(GridEye* ge) {
    furi_hal_i2c_acquire(I2C_BUS);

    furi_check(furi_hal_i2c_write_reg_8(
        I2C_BUS, ge->addr, REGISTER_POWER_CONTROL, POWER__SLEEP_MODE, I2C_TIMEOUT));

    ge->status = GridEyeStatus_Sleep;

    furi_hal_i2c_release(I2C_BUS);
    return 0;
}

int gridEye_wake(GridEye* ge) {
    furi_hal_i2c_acquire(I2C_BUS);

    furi_check(furi_hal_i2c_write_reg_8(
        I2C_BUS, ge->addr, REGISTER_POWER_CONTROL, POWER__NORMAL_MODE, I2C_TIMEOUT));

    ge->status = GridEyeStatus_OK;

    furi_hal_i2c_release(I2C_BUS);
    return 0;
}

int gridEye_update(GridEye* ge) {
    furi_check(ge != NULL);

    //Grab temperatures
    furi_hal_i2c_acquire(I2C_BUS);
    furi_check(furi_hal_i2c_read_mem(
        I2C_BUS, ge->addr, REGISTER_TEMPERATURE_BASE_ADDR, ge->tempData, 128, 100));
    furi_hal_i2c_release(I2C_BUS);

    //Update min and max
    ge->max = gridEye_getTemperature(ge, 0);
    ge->min = ge->max;
    for(uint8_t i = 0; i < 64; i++) {
        float temperature = gridEye_getTemperature(ge, i);
        if(temperature > ge->max) ge->max = temperature;
        if(temperature < ge->min) ge->min = temperature;
    }

    //Create 9 'bins' we can split the data into and calculate where the 'separators' between them should be based on max vs min
    float binSeparators[NUM_TEMPERATURE_BINS];
    float interval = (ge->max - ge->min) / NUM_TEMPERATURE_BINS;
    for(uint8_t i = 0; i < NUM_TEMPERATURE_BINS; i++) binSeparators[i] = ge->min + (interval * i);

    //Sort each of the 64 pixels into its respective bin
    for(uint8_t i = 0; i < 64; i++) {
        float temp = gridEye_getTemperature(ge, i);
        for(uint8_t b = 0; b < NUM_TEMPERATURE_BINS && temp > binSeparators[b]; b++)
            ge->tempDataBins[i] = b;
    }

    return 0;
}

float gridEye_getTemperature(GridEye* ge, uint8_t pixelAddr) {
    furi_check(pixelAddr < 64);
    furi_check(ge != NULL);
    //if(ge->status != GridEyeStatus_OK) return (float)-999;
    uint16_t temp_raw = ((ge->tempData[pixelAddr * 2 + 1] & 0x07) << 8) |
                        ge->tempData[pixelAddr * 2];

    float temperature = ((float)temp_raw) * 0.25;

    if((ge->tempData[pixelAddr * 2 + 1] | 0x08) == 1) //Sign bit check
        temperature = -temperature;

    return temperature;
}

int16_t gridEye_getTemperatureInt(GridEye* ge, uint8_t pixelAddr) {
    return (int16_t)gridEye_getTemperature(ge, pixelAddr);
}

uint8_t gridEye_getTemperatureGrayscale(GridEye* ge, uint8_t pixelAddr) {
    furi_check(ge != NULL);
    furi_check(pixelAddr < 64);
    return ge->tempDataBins[pixelAddr];
}

int gridEye_free(GridEye* ge) {
    furi_assert(ge != NULL);
    gridEye_sleep(ge);
    free(ge);
    return 0;
}
