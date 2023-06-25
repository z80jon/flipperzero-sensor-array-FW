#include "../../sensor_app_i.h"
#include "../../lib/amg8833/amg8833.h"

void sensor_scene_IRCam_on_enter(void* context) {
    furi_assert(context);
    SensorApp* app = context;
    app->Sensor_IRCam->sensor = AMG8833_init(I2C_ADDR_AMG8833, AMG8833FrameRate_10FPS);
    view_dispatcher_switch_to_view(app->view_dispatcher, SensorAppViewIRCam);
}

bool sensor_scene_IRCam_on_event(void* context, SceneManagerEvent event) {
    furi_assert(context);
    SensorApp* app = context;
    AMG8833_update(app->Sensor_IRCam->sensor);
    UNUSED(event);
    return false;
}

void sensor_scene_IRCam_on_exit(void* context) {
    furi_assert(context);
    SensorApp* app = context;
    //Shut down the IR camera and free its memory
    AMG8833_free(app->Sensor_IRCam->sensor);
    app->Sensor_IRCam->sensor = NULL;
}
