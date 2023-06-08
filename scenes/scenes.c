#include "scenes.h"

// Generate scene on_enter handlers array
#define ADD_SCENE(prefix, name, id) prefix##_scene_##name##_on_enter,
void (*const sensor_scene_on_enter_handlers[])(void*) = {
#include "sceneconfigs.h"
};
#undef ADD_SCENE

// Generate scene on_event handlers array
#define ADD_SCENE(prefix, name, id) prefix##_scene_##name##_on_event,
bool (*const sensor_scene_on_event_handlers[])(void* context, SceneManagerEvent event) = {
#include "sceneconfigs.h"
};
#undef ADD_SCENE

// Generate scene on_exit handlers array
#define ADD_SCENE(prefix, name, id) prefix##_scene_##name##_on_exit,
void (*const sensor_scene_on_exit_handlers[])(void* context) = {
#include "sceneconfigs.h"
};
#undef ADD_SCENE

// Initialize scene handlers configuration structure
const SceneManagerHandlers sensor_scene_handlers = {
    .on_enter_handlers = sensor_scene_on_enter_handlers,
    .on_event_handlers = sensor_scene_on_event_handlers,
    .on_exit_handlers = sensor_scene_on_exit_handlers,
    .scene_num = SensorSceneNum,
};
