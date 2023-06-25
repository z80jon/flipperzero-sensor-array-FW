#pragma once

#include "../../sensor_app_i.h"

#include <gui/view.h>

typedef void (*GpioTestOkCallback)(InputType type, void* context);

Sensor_IRCam* view_IRCam_grayscale_alloc(SensorApp* app);

void view_IRCam_grayscale_free(Sensor_IRCam* view_IRCam);

View* view_IRCam_grayscale_get_view(Sensor_IRCam* view_IRCam);

void view_IRCam_grayscale_set_ok_callback(
    Sensor_IRCam* view_IRCam,
    GpioTestOkCallback callback,
    void* context);