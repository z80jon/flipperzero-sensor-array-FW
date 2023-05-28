#pragma once

#include "sensor_app.h"
#include "sensor_items.h"
#include "scenes/sensor_scene.h"
#include "sensor_custom_event.h"
#include "usb_uart_bridge.h"

#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/submenu.h>
#include <notification/notification_messages.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/widget.h>
#include "views/sensor_test.h"
#include "views/sensor_usb_uart.h"
#include "views/sensor_IRCam.h"
#include <assets_icons.h>

struct SensorApp {
    Gui* gui;
    NotificationApp* notifications;
    ViewDispatcher* view_dispatcher;
    SceneManager* scene_manager;
    Widget* widget;

    VariableItemList* var_item_list;
    VariableItem* var_item_flow;
    GpioTest* gpio_test;
    GpioUsbUart* gpio_usb_uart;
    GPIOItems* gpio_items;
    //IRCamItems* IR_cam_items;
    UsbUartBridge* usb_uart_bridge;
    UsbUartConfig* usb_uart_cfg;
};

typedef enum {
    SensorAppViewVarItemList,
    SensorAppViewGpioTest,
    SensorAppViewUsbUart,
    SensorAppViewUsbUartCfg,
    SensorAppViewUsbUartCloseRpc,
    SensorAppViewIRCam
} SensorAppView;
