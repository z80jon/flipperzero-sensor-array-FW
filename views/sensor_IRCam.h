#pragma once

#include "../sensor_items.h"

#include <gui/view.h>

typedef struct GpioTest GpioTest;
typedef void (*GpioTestOkCallback)(InputType type, void* context);

GpioTest* sensor_IRCam_alloc(GPIOItems* gpio_items);

void sensor_IRCam_free(GpioTest* sensor_IRCam);

View* sensor_IRCam_get_view(GpioTest* sensor_IRCam);

void sensor_IRCam_set_ok_callback(GpioTest* sensor_IRCam, GpioTestOkCallback callback, void* context);
