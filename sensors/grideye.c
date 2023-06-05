#include "grideye.h"
#include <furi_hal_i2c.h>
#include <furi_hal_i2c_config.h>
#include <unistd.h>
#include <stdlib.h>
#include <core/check.h>
#include <furi.h>
#include <furi_hal.h>
//Registers

#define DEFAULT_ADDRESS 0x69

#define POWER_CONTROL_REGISTER 0x00
#define POWER__NORMAL_MODE 0x00
#define POWER__SLEEP_MODE 0x10

#define RESET_REGISTER 0x01

#define FRAMERATE_REGISTER 0x02

#define INT_CONTROL_REGISTER 0x03
#define STATUS_REGISTER 0x04
#define STATUS_CLEAR_REGISTER 0x05
#define AVERAGE_REGISTER 0x07
#define INT_LEVEL_REGISTER_UPPER_LSB 0x08
#define INT_LEVEL_REGISTER_UPPER_MSB 0x09
#define INT_LEVEL_REGISTER_LOWER_LSB 0x0A
#define INT_LEVEL_REGISTER_LOWER_MSB 0x0B
#define INT_LEVEL_REGISTER_HYST_LSB 0x0C
#define INT_LEVEL_REGISTER_HYST_MSB 0x0D
#define THERMISTOR_REGISTER_LSB 0x0E
#define THERMISTOR_REGISTER_MSB 0x0F
#define INT_TABLE_REGISTER_INT0 0x10
#define RESERVED_AVERAGE_REGISTER 0x1F
#define TEMPERATURE_REGISTER_START 0x80

/**
 * @brief Initializes comms to the specified AMG88xx GridEye module, 
 * 
 * @param addr the 8-bit (addr<<1 if using 7-bit notation) I2C address of the AMG88XX
 * @param frameRate the framerate (1 or 10FPS) to use
 * @return GridEye an initialized GridEye object with a valid gridEyeStatus, ready to grab a frame
 *                 when gridEye_update is called next.
 */
GridEye* gridEye_init(uint8_t addr, eGridEyeFramerate frameRate) {
    //Make sure the module is there before we do anything
    furi_hal_i2c_acquire(I2C_BUS);
    if(!furi_hal_i2c_is_device_ready(I2C_BUS, addr, I2C_TIMEOUT)) {
        return NULL;
    }

    //Struct setup
    GridEye* ge = (GridEye*)malloc(sizeof(GridEye));
    ge->addr = addr;
    ge->freshData = false;
    ge->status = GridEyeStatus_OK;

    //Release the i2c bus as the function calls will want to acquire it themselves
    furi_hal_i2c_release(I2C_BUS);

    //Make sure we're in normal mode
    gridEye_wake(ge);

    //Set framerate to normal
    furi_assert(gridEye_setFrameRate(ge, frameRate) == 0);

    //TODO setup defaults (add #define in .c), update status

    return ge;
}

/**
 * @brief Updates and returns the status of the current gridEye object
 * 
 * @param ge the GridEye instant to check the status of
 * @return gridEyeStatus the current status (ok, asleep, error state, etc)
 */
eGridEyeStatus gridEye_getStatus(GridEye* ge) {
    furi_hal_i2c_acquire(I2C_BUS);

    //Make sure we can talk to the AMG8833
    if(furi_hal_i2c_is_device_ready(I2C_BUS, ge->addr, I2C_TIMEOUT) == false) {
        ge->status = GridEyeStatus_Error;
        return GridEyeStatus_Error;
    }

    uint8_t status;
    bool furi_hal_i2c_read_reg_8(I2C_BUS, ge->addr, POWER_CONTROL_REGISTER, &status, I2C_TIMEOUT);

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

/**
 * @brief Sets the framerate of the current GridEye object
 * 
 * @param ge the GridEye object
 * @return int 0 if ok, else 1
 */
int gridEye_setFrameRate(GridEye* ge, eGridEyeFramerate frameRate) {
    furi_hal_i2c_acquire(I2C_BUS);

    //Make sure the framerate is valid
    furi_assert(frameRate == GridEyeFrameRate_10FPS || frameRate == GridEyeFrameRate_1FPS);

    //The framerate enums equate to the actual register value to write, so just send them
    //If it doesn't go through, we fail the check.
    furi_assert(
        furi_hal_i2c_write_reg_8(I2C_BUS, ge->addr, FRAMERATE_REGISTER, frameRate, I2C_TIMEOUT));

    //Update the data structure
    ge->frameRate = frameRate;

    furi_hal_i2c_release(I2C_BUS);
    return 0;
}

/**
 * @brief Puts the AMG88xx to sleep to preserve power
 * 
 * @param ge the GridEye instance
 * @return int 0 if ok, else 1
 */
int gridEye_sleep(GridEye* ge) {
    furi_hal_i2c_acquire(I2C_BUS);

    furi_check(furi_hal_i2c_write_reg_8(
        I2C_BUS, ge->addr, POWER_CONTROL_REGISTER, POWER__SLEEP_MODE, I2C_TIMEOUT));

    ge->status = GridEyeStatus_Sleep;

    furi_hal_i2c_release(I2C_BUS);
    return 0;
}

/**
 * @brief Wakes the AMG88xx up from standby
 * 
 * @param ge the GridEye instance
 * @return int 0 if ok, else 1
 */
int gridEye_wake(GridEye* ge) {
    furi_hal_i2c_acquire(I2C_BUS);

    furi_check(furi_hal_i2c_write_reg_8(
        I2C_BUS, ge->addr, POWER_CONTROL_REGISTER, POWER__NORMAL_MODE, I2C_TIMEOUT));

    ge->status = GridEyeStatus_OK;

    furi_hal_i2c_release(I2C_BUS);
    return 0;
}

/**
 * @brief Fetches new data from the AMG88xx. Also updates the status.
 * 
 * @param ge the GridEye instance
 * @return int 0 if ok, else 1
 */
int gridEye_update(GridEye* ge) {
    if(gridEye_getStatus(ge) != GridEyeStatus_OK) return 1;

    furi_hal_i2c_acquire(I2C_BUS);

    furi_assert(furi_hal_i2c_read_mem(
        I2C_BUS, ge->addr, TEMPERATURE_REGISTER_START, ge->tempData, 128, I2C_TIMEOUT));

    furi_hal_i2c_release(I2C_BUS);
    return 0;
}

/**
 * @brief Returns the current value of the specified pixel of the AMG88xx instance as a floating-point value
 * 
 * @param ge 
 * @param pixelAddr the pixel address on a mapping of 0...63, from top left to top right, top+1 left to top+1 right, and so on.
 * @return float the temperature, in Celsius.
 */
float gridEye_getTemperature(GridEye* ge, uint8_t pixelAddr) {
    if(pixelAddr > 63) return -999;
    if(ge->status != GridEyeStatus_OK) return (float)-999;

    float temperature = ((ge->tempData[pixelAddr * 2] & 0x07) << 8) |
                        ge->tempData[pixelAddr * 2 + 1];

    if((ge->tempData[pixelAddr * 2] | 0x08) == 1) //Sign bit check
        temperature = -temperature;

    return temperature;
}

/**
 * @brief Returns the current value of the specified pixel of the AMG88xx instance as an integer
 * 
 * @param ge 
 * @param pixelAddr the pixel address on a mapping of 0...63, from top left to top right, top+1 left to top+1 right, and so on.
 * @return int16_t the temperature as a whole integer, in Celsius.
 */
int16_t gridEye_getTemperatureInt(GridEye* ge, uint8_t pixelAddr) {
    return (int16_t)gridEye_getTemperature(ge, pixelAddr);
}

/**
 * @brief Puts the GridEye to sleep and frees up the allocated GridEye object
 * 
 * @param ge the GridEye instance
 * @return int 0 if ok, else 1
 */
int gridEye_free(GridEye* ge) {
    if(ge == NULL) return 1;

    free(ge->tempData);
    free(ge);
    return 0;
}
