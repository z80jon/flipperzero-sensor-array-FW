#include "../sensor_app_i.h"
#include <furi_hal_power.h>
#include <furi_hal_usb.h>
#include <dolphin/dolphin.h>

enum GpioItem {
    GpioItemUsbUart,
    GpioItemTest,
    GpioItemOtg,
    SensorItemIRCam
};

enum GpioOtg {
    GpioOtgOff,
    GpioOtgOn,
    GpioOtgSettingsNum,
};

const char* const gpio_otg_text[GpioOtgSettingsNum] = {
    "OFF",
    "ON",
};

static void gpio_scene_start_var_list_enter_callback(void* context, uint32_t index) {
    furi_assert(context);
    SensorApp* app = context;
    switch(index) {
        case GpioItemTest: 
            view_dispatcher_send_custom_event(app->view_dispatcher, GpioStartEventManualControl);
            break;
        case GpioItemUsbUart:
            view_dispatcher_send_custom_event(app->view_dispatcher, GpioStartEventUsbUart);
            break;
        case SensorItemIRCam:
            view_dispatcher_send_custom_event(app->view_dispatcher, SensorItemEventStartIRCam);
            break;

    }
}

static void gpio_scene_start_var_list_change_callback(VariableItem* item) {
    SensorApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, gpio_otg_text[index]);
    if(index == GpioOtgOff) {
        view_dispatcher_send_custom_event(app->view_dispatcher, GpioStartEventOtgOff);
    } else if(index == GpioOtgOn) {
        view_dispatcher_send_custom_event(app->view_dispatcher, GpioStartEventOtgOn);
    }
}

void gpio_scene_start_on_enter(void* context) {
    SensorApp* app = context;
    VariableItemList* var_item_list = app->var_item_list;

    VariableItem* item;
    variable_item_list_set_enter_callback(
        var_item_list, gpio_scene_start_var_list_enter_callback, app);

    variable_item_list_add(var_item_list, "USB-UART Bridge", 0, NULL, NULL);

    variable_item_list_add(var_item_list, "GPIO Manual Control", 0, NULL, NULL);

    item = variable_item_list_add(
        var_item_list,
        "5V on GPIO",
        GpioOtgSettingsNum,
        gpio_scene_start_var_list_change_callback,
        app);
    if(furi_hal_power_is_otg_enabled()) {
        variable_item_set_current_value_index(item, GpioOtgOn);
        variable_item_set_current_value_text(item, gpio_otg_text[GpioOtgOn]);
    } else {
        variable_item_set_current_value_index(item, GpioOtgOff);
        variable_item_set_current_value_text(item, gpio_otg_text[GpioOtgOff]);
    }

    variable_item_list_add(var_item_list, "IR Camera", 0, NULL, NULL);

    variable_item_list_set_selected_item(
        var_item_list, scene_manager_get_scene_state(app->scene_manager, GpioSceneStart));

    view_dispatcher_switch_to_view(app->view_dispatcher, SensorAppViewVarItemList);
}

bool gpio_scene_start_on_event(void* context, SceneManagerEvent event) {
    SensorApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == GpioStartEventOtgOn) {
            furi_hal_power_enable_otg();
        } else if(event.event == GpioStartEventOtgOff) {
            furi_hal_power_disable_otg();
        } else if(event.event == GpioStartEventManualControl) {
            scene_manager_set_scene_state(app->scene_manager, GpioSceneStart, GpioItemTest);
            scene_manager_next_scene(app->scene_manager, GpioSceneTest);
        } else if(event.event == GpioStartEventUsbUart) {
            scene_manager_set_scene_state(app->scene_manager, GpioSceneStart, GpioItemUsbUart);
            if(!furi_hal_usb_is_locked()) {
                DOLPHIN_DEED(DolphinDeedGpioUartBridge);
                scene_manager_next_scene(app->scene_manager, GpioSceneUsbUart);
            } else {
                scene_manager_next_scene(app->scene_manager, GpioSceneUsbUartCloseRpc);
            }
        } else if(event.event == SensorItemEventStartIRCam) {
            //scene_manager_set_scene_state(app->scene_manager, GpioSceneStart, ScienceItemIRCamViewer)
        }
        consumed = true;
    }
    return consumed;
}

void gpio_scene_start_on_exit(void* context) {
    SensorApp* app = context;
    variable_item_list_reset(app->var_item_list);
}
