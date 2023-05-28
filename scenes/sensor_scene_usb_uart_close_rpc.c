#include "../sensor_app_i.h"
#include "../sensor_custom_event.h"

void sensor_scene_usb_uart_close_rpc_on_enter(void* context) {
    SensorApp* app = context;

    widget_add_icon_element(app->widget, 78, 0, &I_ActiveConnection_50x64);
    widget_add_string_multiline_element(
        app->widget, 3, 2, AlignLeft, AlignTop, FontPrimary, "Connection\nis active!");
    widget_add_string_multiline_element(
        app->widget,
        3,
        30,
        AlignLeft,
        AlignTop,
        FontSecondary,
        "Disconnect from\nPC or phone to\nuse this function.");

    view_dispatcher_switch_to_view(app->view_dispatcher, SensorAppViewUsbUartCloseRpc);
}

bool sensor_scene_usb_uart_close_rpc_on_event(void* context, SceneManagerEvent event) {
    SensorApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == GpioCustomEventErrorBack) {
            if(!scene_manager_previous_scene(app->scene_manager)) {
                scene_manager_stop(app->scene_manager);
                view_dispatcher_stop(app->view_dispatcher);
            }
            consumed = true;
        }
    }
    return consumed;
}

void sensor_scene_usb_uart_close_rpc_on_exit(void* context) {
    SensorApp* app = context;
    widget_reset(app->widget);
}
