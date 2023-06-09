#pragma once

#include "sensor_app.h"
#include "scenes/scenes.h"
#include "sensor_custom_event.h"
#include "sensors/grideye.h"

#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/submenu.h>
#include <notification/notification_messages.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/widget.h>
#include <assets_icons.h>

typedef struct {
    View* view;
    void* context;
    GridEye* ge;
} SensorIRCam;

struct SensorApp {
    Gui* gui;
    NotificationApp* notifications;
    ViewDispatcher* view_dispatcher;
    SceneManager* scene_manager;
    Widget* widget;

    VariableItemList* var_item_list;
    VariableItem* var_item_flow;

    //Sensor-specific params
    SensorIRCam* SensorIRCam;
};

typedef enum { SensorAppViewMenu, SensorAppViewIRCam, SensorAppViewSettings } SensorAppView;
