#include "../sensor_app_i.h"
#include <furi_hal_power.h>
#include <furi_hal_usb.h>
#include <dolphin/dolphin.h>
#include "../utils.h"

enum MenuItem {
    MenuItemIRCam,
    MenuItemWeatherGasSensor,
    MenuItemTOFDepth,
    MenuItemIMU,
    MenuItemSpectrometer,
    MenuItemSettings
};

static void sensor_scene_menu_var_list_enter_callback(void* context, uint32_t index) {
    furi_assert(context);
    SensorApp* app = context;
    switch(index) {
    case MenuItemIRCam:
        view_dispatcher_send_custom_event(app->view_dispatcher, SensorItemEventStartIRCam);
        break;
    case MenuItemWeatherGasSensor:
        view_dispatcher_send_custom_event(app->view_dispatcher, SensorItemEventStartWeatherSensor);
        break;
    case MenuItemTOFDepth:
        view_dispatcher_send_custom_event(app->view_dispatcher, SensorItemEventStartDepthSensor);
        break;
    case MenuItemIMU:
        view_dispatcher_send_custom_event(app->view_dispatcher, SensorItemEventStartIMU);
        break;
    case MenuItemSpectrometer:
        view_dispatcher_send_custom_event(app->view_dispatcher, SensorItemEventStartSpectrometer);
        break;
    case MenuItemSettings:
        //NYI view_dispatcher_send_custom_event(app->view_dispatcher, SensorItemEventStartIRCam);
        //TODO implement
        break;
    }
}

// static void sensor_scene_menu_var_list_change_callback(VariableItem* item) {
//     SensorApp* app = variable_item_get_context(item);
//     uint8_t index = variable_item_get_current_value_index(item);
//     UNUSED(index);
//     UNUSED(app);
// }

#define I2C_ADDR_BME688 0x76 << 1
#define I2C_ADDR_VL53L5CX 0x52 << 1
#define I2C_ADDR_ICM_20948 0x68 << 1
#define I2C_ADDR_AS7343 0x39 << 1

void sensor_scene_menu_on_enter(void* context) {
    SensorApp* app = context;
    VariableItemList* var_item_list = app->var_item_list;

    variable_item_list_set_enter_callback(
        var_item_list, sensor_scene_menu_var_list_enter_callback, app);

    if(utils_isI2CDeviceReady(I2C_ADDR_AMG8833)) {
        variable_item_list_add(var_item_list, "8x8 Thermal Imager", 0, NULL, NULL);
    } else {
        variable_item_list_add(var_item_list, "(unavailable)8x8 Thermal Imager", 0, NULL, NULL);
    }

    if(utils_isI2CDeviceReady(I2C_ADDR_BME688)) {
        variable_item_list_add(var_item_list, "Weather/Gas Sensors", 0, NULL, NULL);
    } else {
        variable_item_list_add(var_item_list, "(unavailable)Weather/Gas Sensors", 0, NULL, NULL);
    }

    if(utils_isI2CDeviceReady(I2C_ADDR_VL53L5CX)) {
        variable_item_list_add(var_item_list, "8x8 Distance Imager", 0, NULL, NULL);
    } else {
        variable_item_list_add(var_item_list, "(unavailable)8x8 Distance Imager", 0, NULL, NULL);
    }

    if(utils_isI2CDeviceReady(I2C_ADDR_ICM_20948)) {
        variable_item_list_add(var_item_list, "9-Axis IMU", 0, NULL, NULL);
    } else {
        variable_item_list_add(var_item_list, "(unavailable)9-Axis IMU", 0, NULL, NULL);
    }

    if(utils_isI2CDeviceReady(I2C_ADDR_AS7343)) {
        variable_item_list_add(var_item_list, "Light Spectrometer", 0, NULL, NULL);
    } else {
        variable_item_list_add(var_item_list, "(unavailable)Light Spectrometer", 0, NULL, NULL);
    }

    variable_item_list_add(var_item_list, "Settings", 0, NULL, NULL);

    variable_item_list_set_selected_item(
        var_item_list, scene_manager_get_scene_state(app->scene_manager, SensorSceneMenu));

    view_dispatcher_switch_to_view(app->view_dispatcher, SensorAppViewMenu);
}

bool sensor_scene_menu_on_event(void* context, SceneManagerEvent event) {
    SensorApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        // if(event.event == GpioStartEventOtgOn) {
        //     furi_hal_power_enable_otg();
        // } else if(event.event == GpioStartEventOtgOff) {
        //     furi_hal_power_disable_otg();
        // } else if(event.event == GpioStartEventManualControl) {
        //     scene_manager_set_scene_state(app->scene_manager, GpioSceneStart, GpioItemTest);
        //     scene_manager_next_scene(app->scene_manager, GpioSceneTest);
        // } else if(event.event == GpioStartEventUsbUart) {
        //     scene_manager_set_scene_state(app->scene_manager, GpioSceneStart, GpioItemUsbUart);
        //     if(!furi_hal_usb_is_locked()) {
        //         DOLPHIN_DEED(DolphinDeedGpioUartBridge);
        //         scene_manager_next_scene(app->scene_manager, GpioSceneUsbUart);
        //     } else {
        //         scene_manager_next_scene(app->scene_manager, GpioSceneUsbUartCloseRpc);
        //     }
        if(event.event == SensorItemEventStartDepthSensor) {
            scene_manager_set_scene_state(app->scene_manager, SensorSceneMenu, MenuItemTOFDepth);
            scene_manager_next_scene(app->scene_manager, SensorSceneTOFDepth);
        }
        if(event.event == SensorItemEventStartIRCam && utils_isI2CDeviceReady(I2C_ADDR_AMG8833)) {
            scene_manager_set_scene_state(app->scene_manager, SensorSceneMenu, MenuItemIRCam);
            scene_manager_next_scene(app->scene_manager, SensorSceneIRCam);
        }
        consumed = true;
    }
    return consumed;
}

void sensor_scene_menu_on_exit(void* context) {
    SensorApp* app = context;
    variable_item_list_reset(app->var_item_list);
}
