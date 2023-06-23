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
    //If the GridEye is already initialized, don't bother resetting it
    //TODO change this so we init/deinit as we start and stop the app? Or do we want to perma-alloc everything?
    if(app->SensorIRCam->ge == NULL)
        app->SensorIRCam->ge = gridEye_init(I2C_ADDR_AMG8833, GridEyeFrameRate_10FPS);
    //view_IRCam_grayscale_set_ok_callback(app->SensorIRCam, sensor_scene_IRCam_ok_callback, app);
    //TODO investigate
    view_dispatcher_switch_to_view(app->view_dispatcher, SensorAppViewIRCam);
}

bool sensor_scene_IRCam_on_event(void* context, SceneManagerEvent event) {
    furi_assert(context);
    SensorApp* app = context;
    gridEye_update(app->SensorIRCam->ge);
    UNUSED(event);
    return false;
}

void sensor_scene_IRCam_on_exit(void* context) {
    furi_assert(context);
    SensorApp* app = context;
    notification_message(app->notifications, &sequence_display_backlight_enforce_on);
    UNUSED(app);
}
