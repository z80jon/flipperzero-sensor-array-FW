#include "../sensor_app_i.h"

void sensor_scene_IRCam_ok_callback(InputType type, void* context) {
    furi_assert(context);
    SensorApp* app = context;

    if(type == InputTypePress) {
        notification_message(app->notifications, &sequence_set_blue_255);
    } else if(type == InputTypeRelease) {
        notification_message(app->notifications, &sequence_reset_green);
    }
}

void sensor_scene_IRCam_on_enter(void* context) {
    furi_assert(context);
    SensorApp* app = context;
    app->SensorIRCam->ge = gridEye_init(0x00, GridEyeFrameRate_10FPS);
    //TODO change address to correct one
    //TODO initialize grideye
    //view_IRCam_set_ok_callback(app->SensorIRCam, sensor_scene_IRCam_ok_callback, app);
    //TODO investigate
    view_dispatcher_switch_to_view(app->view_dispatcher, SensorAppViewIRCam);
}

bool sensor_scene_IRCam_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void sensor_scene_IRCam_on_exit(void* context) {
    furi_assert(context);
    SensorApp* app = context;
    //gpio_items_configure_all_pins(app->gpio_items, GpioModeAnalog);
    UNUSED(app);
    //TODO fixme
}
