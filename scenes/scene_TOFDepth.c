#include "../sensor_app_i.h"
#include "../sensors/vl53l5cx/vl53l5cx_api.h"

/**
 * @brief Parses fresh data from the sensor, extracting the min and max values as well as populating a grayscale
 *        table.
 * 
 * @param tof TOFSensor struct pointer
 */
static void process_data(TOFSensor* tof);

void sensor_scene_TOFDepth_ok_callback(InputType type, void* context) {
    furi_assert(context);
    SensorApp* app = context;

    if(type == InputTypePress) {
        notification_message(app->notifications, &sequence_set_blue_255);
    } else if(type == InputTypeRelease) {
        notification_message(app->notifications, &sequence_reset_green);
    }
}

void sensor_scene_TOFDepth_on_enter(void* context) {
    furi_assert(context);
    SensorApp* app = context;

    //TOF Sensor setup

    VL53L5CX_Configuration* tofConfig = app->SensorTOFDepth->tof->TOFConfiguration;
    furi_check(tofConfig != NULL);

    vl53l5cx_init(tofConfig);
    vl53l5cx_start_ranging(tofConfig);
    vl53l5cx_set_resolution(tofConfig, VL53L5CX_RESOLUTION_8X8);

    view_dispatcher_switch_to_view(app->view_dispatcher, SensorAppViewTOFDepth);
}

bool sensor_scene_TOFDepth_on_event(void* context, SceneManagerEvent event) {
    SensorApp* app = context;
    TOFSensor* tof = app->SensorTOFDepth->tof;
    UNUSED(event);

    uint8_t ready = 0;
    furi_assert(vl53l5cx_check_data_ready(tof->TOFConfiguration, &ready));
    if(ready) {
        notification_message(app->notifications, &sequence_blink_blue_100);
        vl53l5cx_get_ranging_data(tof->TOFConfiguration, tof->TOFResultsData);
        process_data(tof);
    }

    return false;
}

void sensor_scene_TOFDepth_on_exit(void* context) {
    furi_assert(context);
    SensorApp* app = context;
    TOFSensor* tof = app->SensorTOFDepth->tof;
    vl53l5cx_stop_ranging(tof->TOFConfiguration);
    //gpio_items_configure_all_pins(app->gpio_items, GpioModeAnalog);
    UNUSED(app);
    //TODO fixme
}

void process_data(TOFSensor* tof) {
    furi_check(tof);
    int16_t* distances = tof->TOFResultsData->distance_mm;
    tof->min = distances[0];
    tof->max = distances[0];

    //Find min/max
    for(uint8_t i = 0; i < 64; i++) {
        if(distances[i] > tof->max) tof->max = distances[i];
        if(distances[i] < tof->min) tof->min = distances[i];
    }

    //Create 9 'bins' we can split the data into and calculate where the 'separators' between them should be based on max vs min
    int16_t binSeparators[NUM_TEMPERATURE_BINS];
    float interval = (tof->min - tof->max) / NUM_TEMPERATURE_BINS;
    for(uint8_t i = 0; i < NUM_TEMPERATURE_BINS; i++) binSeparators[i] = tof->min + (interval * i);

    //Sort each of the 64 pixels into its respective bin
    for(uint8_t i = 0; i < 64; i++) {
        tof->grayscale[i] = 0;
        for(uint8_t bin = 1; bin < NUM_TEMPERATURE_BINS; bin++) {
            if(distances[i] > binSeparators[bin - 1]) {
                tof->grayscale[i] = bin;
            }
        }
    }
}
