#pragma once

typedef enum {
    SensorItemEventStartIRCam = 0,
    SensorItemEventStartWeatherSensor,
    SensorItemEventStartDepthSensor,
    SensorItemEventStartIMU,
    SensorItemEventStartSpectrometer,
    SensorItemEventStartSettings
} SensorCustomEvent;
