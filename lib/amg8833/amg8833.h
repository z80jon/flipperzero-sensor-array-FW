#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <furi_hal_i2c.h>
#include <furi_hal_i2c_config.h>
#include <furi_hal_i2c_types.h>
#include <furi.h>
#include <furi_hal.h>

#define I2C_ADDR_AMG8833 0x69 << 1

typedef enum {
    AMG8833Status_OK, //< AMG8833 is operating normally
    AMG8833Status_Sleep, //< AMG8833 is in sleep mode and will not take any images
    AMG8833Status_Error //< Error state / uninitialized. Unused.
} eAMG8833Status;

typedef enum {
    AMG8833FrameRate_10FPS = 0x00, //< AMG8833 will capture 10 frames per second
    AMG8833FrameRate_1FPS = 0x01 //< AMG8833 will capture 1 frame per second
} eAMG8833Framerate;

typedef struct {
    eAMG8833Status status; //< Current status of the AMG8833 (sleeping, capturing, etc)
    uint8_t addr; //< i2c address of the device

    eAMG8833Framerate frameRate; //< Current rate at which new images will be captured
    uint8_t tempData[128]; //< Unprocessed 12-bit temp data. Check datasheet for details.
    uint8_t tempDataBins[64]; //< Processed temperature data into 9-level grayscale bins
    float min; //< The min value seen in the current frame of temperatures
    float max; //< The max value seen in the current frame of temperatures
    bool freshData; //< Set to 'true' when fresh data is captured. Cleared next cycle.
} AMG8833;

/**
 * @brief Initializes comms to the specified AMG88xx GridEye module, 
 * 
 * @param addr the 8-bit (addr<<1 if using 7-bit notation) I2C address of the AMG88XX
 * @param frameRate the framerate (1 or 10FPS) to use
 * @return AMG8833 an initialized AMG8833 object with a valid AMG8833Status, ready to grab a frame
 *                 when AMG8833_update is called next.
 */
AMG8833* AMG8833_init(uint8_t addr, eAMG8833Framerate frameRate);

/**
 * @brief Updates and returns the status of the current AMG8833 object
 * 
 * @param sensor the AMG8833 instance to check the status of
 * @return AMG8833Status the current status (ok, asleep, error state, etc)
 */
eAMG8833Status AMG8833_getStatus(AMG8833* sensor);

/**
 * @brief Sets the framerate of the current AMG8833 object
 * 
 * @param sensor the AMG8833 object
 * @return int 0 if ok, else 1
 */
int AMG8833_setFrameRate(AMG8833* sensor, eAMG8833Framerate frameRate);

/**
 * @brief Puts the AMG88xx to sleep to preserve power
 * 
 * @param sensor the AMG8833 instance
 * @return int 0 if ok, else 1
 */
int AMG8833_sleep(AMG8833* sensor);

/**
 * @brief Wakes the AMG88xx up from standby
 * 
 * @param sensor the AMG8833 instance
 * @return int 0 if ok, else 1
 */
int AMG8833_wake(AMG8833* sensor);

/**
 * @brief Fetches new data from the AMG88xx. Also updates the status.
 * 
 * @param sensor the AMG8833 instance
 * @return int 0 if ok, else 1
 */
int AMG8833_update(AMG8833* sensor);

/**
 * @brief Returns the current value of the specified pixel of the AMG88xx instance as a floating-point value
 * 
 * @param sensor the AMG8833 instance
 * @param pixelAddr the pixel address on a mapping of 0...63, from top left to top right, top+1 left to top+1 right, and so on.
 * @return float the temperature, in Celsius.
 */
float AMG8833_getTemperature(AMG8833* sensor, uint8_t pixelAddr);

/**
 * @brief Returns the current value of the specified pixel of the AMG88xx instance as an integer
 * 
 * @param sensor the AMG8833 instance
 * @param pixelAddr the pixel address on a mapping of 0...63, from top left to top right, top+1 left to top+1 right, and so on.
 * @return int16_t the temperature as a whole integer, in Celsius.
 */
int16_t AMG8833_getTemperatureInt(AMG8833* sensor, uint8_t pixelAddr);

/**
 * @brief Returns the current value of the specified pixel of the AMG88xx instance as a value of
 *        0-8 for grayscale rendering (0 = min, 8 = max). It uses the min and max temperatures of
 *        the current frame to create 9 regions of temperatures, which it uses to determine which
 *        region the specified pixel should fall into.
 * 
 * @param sensor the AMG8833 instance
 * @param pixelAddr the pixel address on a mapping of 0...63, from top left to top right, top+1 left to top+1 right, and so on.
 * @return uint8_t the temperature normalized to a scale of 0-8 (see brief)
 */
uint8_t AMG8833_getTemperatureGrayscale(AMG8833* sensor, uint8_t pixelAddr);

/**
 * @brief Puts the AMG8833 to sleep and frees up the allocated AMG8833 object
 * 
 * @param sensor the AMG8833 instance
 * @return int 0 if ok, else 1
 */
int AMG8833_free(AMG8833* sensor);
