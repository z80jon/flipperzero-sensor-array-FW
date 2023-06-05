#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <furi_hal_i2c.h>

typedef enum {
    GridEyeStatus_Error, //< Error state / uninitialized. Unused.
    c, //< GridEye is operating normally
    GridEyeStatus_Sleep //< GridEye is in sleep mode and will not take any images
} gridEyeStatus;

typedef enum {
    GridEyeFrameRate_10FPS = 0x00, //< GridEye will capture 10 frames per second
    GridEyeFrameRate_1FPS = 0x01 //< GridEye will capture 1 frame per second
} gridEyeFramerate;

typedef struct {
    gridEyeStatus status; //< Current status of the GridEye (sleeping, capturing, etc)
    uint8_t addr; //< i2c address of the device

    gridEyeFramerate frameRate; //< Current rate at which new images will be captured
    uint16_t tempData[64]; //< Unprocessed 12-bit temp data. Check datasheet for details.
    bool freshData; //< Set to 'true' when fresh data is captured. Cleared next cycle.
} GridEye;

#define I2C_BUS &furi_hal_i2c_handle_external
#define I2C_TIMEOUT 10

#define BH1750_ADDRESS (0x23 << 1)

#define BH1750_POWER_DOWN 0x00
#define BH1750_POWER_ON 0x01
#define BH1750_RESET 0x07
#define BH1750_DEFAULT_MTREG 69
#define BH1750_DEFAULT_MODE ONETIME_HIGH_RES_MODE

#define BH1750_CONVERSION_FACTOR 1.2

/**
 * @brief Initializes comms to the specified AMG88xx GridEye module, 
 * 
 * @param addr the 8-bit (addr<<1 if using 7-bit notation) I2C address of the AMG88XX
 * @param frameRate the framerate (1 or 10FPS) to use
 * @return GridEye an initialized GridEye object with a valid gridEyeStatus, ready to grab a frame
 *                 when gridEye_update is called next.
 */
GridEye gridEye_init(uint8_t addr, gridEyeFramerate frameRate);

/**
 * @brief Updates and returns the status of the current gridEye object
 * 
 * @param ge the GridEye instant to check the status of
 * @return gridEyeStatus the current status (ok, asleep, error state, etc)
 */
gridEyeStatus gridEye_getStatus(GridEye ge);

/**
 * @brief Sets the framerate of the current GridEye object
 * 
 * @param ge the GridEye object
 * @return int 0 if ok, else 1
 */
int gridEye_setFrameRate(GridEye ge, enum gridEyeFramerate);

/**
 * @brief Puts the AMG88xx to sleep to preserve power
 * 
 * @param ge the GridEye instance
 * @return int 0 if ok, else 1
 */
int gridEye_sleep(GridEye ge);

/**
 * @brief Wakes the AMG88xx up from standby
 * 
 * @param ge the GridEye instance
 * @return int 0 if ok, else 1
 */
int gridEye_wake(GridEye ge);

/**
 * @brief Fetches new data from the AMG88xx. Also updates the status.
 * 
 * @param ge the GridEye instance
 * @return int 0 if ok, else 1
 */
int gridEye_update(GridEye ge);

/**
 * @brief Returns the current value of the specified pixel of the AMG88xx instance as a floating-point value
 * 
 * @param ge 
 * @param pixelAddr the pixel address on a mapping of 0...63, from top left to top right, top+1 left to top+1 right, and so on.
 * @return float the temperature, in Celsius.
 */
float gridEye_getTemperature(GridEye ge, uint8_t addr);

/**
 * @brief Returns the current value of the specified pixel of the AMG88xx instance as an integer
 * 
 * @param ge 
 * @param pixelAddr the pixel address on a mapping of 0...63, from top left to top right, top+1 left to top+1 right, and so on.
 * @return int16_t the temperature as a whole integer, in Celsius.
 */
int16_t gridEye_getTemperatureInt(GridEye ge, uint8_t pixelAddr);

/**
 * @brief Puts the GridEye to sleep and frees up the allocated GridEye object
 * 
 * @param ge the GridEye instance
 * @return int 0 if ok, else 1
 */
int gridEye_free(GridEye ge);
