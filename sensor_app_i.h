#pragma once

#include "sensor_app.h"
#include "scenes/scenes.h"
#include "sensor_custom_event.h"
#include "sensors/grideye.h"
#include "sensors/vl53l5cx/vl53l5cx_api.h"

#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/submenu.h>
#include <notification/notification_messages.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/widget.h>
#include <assets_icons.h>

//Number of temperature 'bins' (for grayscale data generation)
#define NUM_TEMPERATURE_BINS 9

//I2C bus settings
#define I2C_TIMEOUT 10
#define I2C_TIMEOUT_LONG 100
#define I2C_BUS &furi_hal_i2c_handle_external

#define I2C_ADDR_VL53L5CX 0x52
#define I2C_ADDR_BME688 0x76 << 1
#define I2C_ADDR_ICM_20948 0x68 << 1
#define I2C_ADDR_AS7343 0x39 << 1

typedef struct {
    View* view;
    void* context;
    GridEye* ge;
} SensorIRCam;

typedef struct {
    VL53L5CX_Configuration* TOFConfiguration;
    VL53L5CX_ResultsData* TOFResultsData;
    int16_t min;
    int16_t max;
    uint8_t grayscale[64];
} TOFSensor;

typedef struct {
    View* view;
    void* context;
    TOFSensor* tof;
} SensorTOFDepth;

struct SensorApp {
    Gui* gui;
    NotificationApp* notifications;
    ViewDispatcher* view_dispatcher;
    SceneManager* scene_manager;
    Widget* widget;

    VariableItemList* var_item_list;

    //Sensor-specific params
    SensorIRCam* SensorIRCam;
    SensorTOFDepth* SensorTOFDepth;
};

typedef enum {
    SensorAppViewMenu, //< Default view when app is launched. Shows selection of which sensor to display data from
    SensorAppViewIRCam, //< IR Camera (8x8 pixel) AMG8833 viewer. Grayscale, basic readouts as well.
    SensorAppViewTOFDepth,
    SensorAppViewSpectrometer,
    SensorAppViewIMU,
    SensorAppViewWeatherSensor,
    SensorAppViewSettings //< NYI settings
} SensorAppView;
