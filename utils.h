#pragma once

#include <stdint.h>

/**
 * @brief Checks the I2C bus to see if a device with the specified address responds when queried
 * 
 * @param addr the address of the device to check for
 * @return true the device was found
 * @return false the device was not found
 */
bool utils_isI2CDeviceReady(uint8_t addr);

/**
 * @brief Given an array of input data, bin it into equally sized bins and store the number bin each should go
 *        in in the array binData.
 * 
 * @param inputData an input array of data numElements long
 * @param binData an array numElements long to output the binning information into
 * @param numElements the number of elements in the array "inputData" and "binData"
 * @param numBins 
 * @param highBinHighData
 */
void utils_binFloats(
    float* inputData,
    uint8_t* binData,
    uint8_t numElements,
    uint8_t numBins,
    bool highBinHighData);