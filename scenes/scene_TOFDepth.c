#include "../sensor_app_i.h"
#include "../sensors/vl53l5cx/vl53l5cx_api.h"

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
    //app->SensorTOFDepth->ge = gridEye_init(I2C_ADDR_AMG8833, GridEyeFrameRate_10FPS);
    //view_TOFDepth_set_ok_callback(app->SensorTOFDepth, sensor_scene_TOFDepth_ok_callback, app);
    //TODO investigate
    view_dispatcher_switch_to_view(app->view_dispatcher, SensorAppViewTOFDepth);
}

bool sensor_scene_TOFDepth_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void sensor_scene_TOFDepth_on_exit(void* context) {
    furi_assert(context);
    SensorApp* app = context;
    //gpio_items_configure_all_pins(app->gpio_items, GpioModeAnalog);
    UNUSED(app);
    //TODO fixme
}
