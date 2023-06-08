#pragma once

#include "../sensor_app_i.h"

#include <gui/view.h>

typedef void (*GpioTestOkCallback)(InputType type, void* context);

SensorIRCam* sensor_IRCam_alloc(SensorApp* app);

void sensor_IRCam_free(SensorIRCam* sensor_IRCam);

View* sensor_IRCam_get_view(SensorIRCam* sensor_IRCam);

void sensor_IRCam_set_ok_callback(
    SensorIRCam* sensor_IRCam,
    GpioTestOkCallback callback,
    void* context);