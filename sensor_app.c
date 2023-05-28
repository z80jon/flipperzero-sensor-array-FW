#include "sensor_app_i.h"

#include <furi.h>
#include <furi_hal.h>

static bool gpio_app_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    SensorApp* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

static bool gpio_app_back_event_callback(void* context) {
    furi_assert(context);
    SensorApp* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

static void gpio_app_tick_event_callback(void* context) {
    furi_assert(context);
    SensorApp* app = context;
    scene_manager_handle_tick_event(app->scene_manager);
}

SensorApp* gpio_app_alloc() {
    SensorApp* app = malloc(sizeof(SensorApp));

    app->gui = furi_record_open(RECORD_GUI);
    app->gpio_items = gpio_items_alloc();

    app->view_dispatcher = view_dispatcher_alloc();
    app->scene_manager = scene_manager_alloc(&sensor_scene_handlers, app);
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);

    view_dispatcher_set_custom_event_callback(
        app->view_dispatcher, gpio_app_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, gpio_app_back_event_callback);
    view_dispatcher_set_tick_event_callback(
        app->view_dispatcher, gpio_app_tick_event_callback, 100);

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    app->notifications = furi_record_open(RECORD_NOTIFICATION);

    app->var_item_list = variable_item_list_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        SensorAppViewVarItemList,
        variable_item_list_get_view(app->var_item_list));
    app->gpio_test = gpio_test_alloc(app->gpio_items);
    view_dispatcher_add_view(
        app->view_dispatcher, SensorAppViewGpioTest, gpio_test_get_view(app->gpio_test));

    app->widget = widget_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, SensorAppViewUsbUartCloseRpc, widget_get_view(app->widget));

    app->gpio_usb_uart = gpio_usb_uart_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, SensorAppViewUsbUart, gpio_usb_uart_get_view(app->gpio_usb_uart));

    view_dispatcher_add_view(
        app->view_dispatcher,
        SensorAppViewUsbUartCfg,
        variable_item_list_get_view(app->var_item_list));

    view_dispatcher_add_view(
        app->view_dispatcher,
        SensorAppViewIRCam,
        gpio_test_get_view(app->gpio_test));

    scene_manager_next_scene(app->scene_manager, GpioSceneStart);

    return app;
}

void gpio_app_free(SensorApp* app) {
    furi_assert(app);

    // Views
    view_dispatcher_remove_view(app->view_dispatcher, SensorAppViewVarItemList);
    view_dispatcher_remove_view(app->view_dispatcher, SensorAppViewGpioTest);
    view_dispatcher_remove_view(app->view_dispatcher, SensorAppViewUsbUart);
    view_dispatcher_remove_view(app->view_dispatcher, SensorAppViewUsbUartCfg);
    view_dispatcher_remove_view(app->view_dispatcher, SensorAppViewUsbUartCloseRpc);
    variable_item_list_free(app->var_item_list);
    widget_free(app->widget);
    gpio_test_free(app->gpio_test);
    gpio_usb_uart_free(app->gpio_usb_uart);

    // View dispatcher
    view_dispatcher_free(app->view_dispatcher);
    scene_manager_free(app->scene_manager);

    // Close records
    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_NOTIFICATION);

    gpio_items_free(app->gpio_items);
    free(app);
}

int32_t sensor_app(void* p) {
    UNUSED(p);
    SensorApp* gpio_app = gpio_app_alloc();

    view_dispatcher_run(gpio_app->view_dispatcher);

    gpio_app_free(gpio_app);

    return 0;
}
