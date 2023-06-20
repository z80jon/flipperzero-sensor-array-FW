#pragma once

#include "../sensor_app_i.h"

#include <gui/view.h>

typedef void (*GpioTestOkCallback)(InputType type, void* context);

SensorIRCam* view_IRCam_grayscale_alloc(SensorApp* app);

void view_IRCam_grayscale_free(SensorIRCam* view_IRCam);

View* view_IRCam_grayscale_get_view(SensorIRCam* view_IRCam);

void view_IRCam_grayscale_set_ok_callback(
    SensorIRCam* view_IRCam,
    GpioTestOkCallback callback,
    void* context);