#pragma once

#include "../../sensor_app_i.h"

#include <gui/view.h>

typedef void (*GpioTestOkCallback)(InputType type, void* context);

Sensor_TOFDepth* view_TOFDepth_grayscale_alloc(SensorApp* app);

void view_TOFDepth_grayscale_free(Sensor_TOFDepth* view_TOFDepth);

View* view_TOFDepth_grayscale_get_view(Sensor_TOFDepth* view_TOFDepth);

void view_TOFDepth_grayscale_set_ok_callback(
    Sensor_TOFDepth* view_TOFDepth,
    GpioTestOkCallback callback,
    void* context);