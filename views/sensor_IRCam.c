#include "sensor_IRCam.h"

#include <gui/elements.h>

static bool sensor_IRCam_process_left(SensorIRCam* sensor_IRCam);
static bool sensor_IRCam_process_right(SensorIRCam* sensor_IRCam);
static bool sensor_IRCam_process_ok(SensorIRCam* sensor_IRCam, InputEvent* event);

static void sensor_IRCam_draw_callback(Canvas* canvas, void* _model) {
    //     GpioTestModel* model = _model;
    //     canvas_set_font(canvas, FontPrimary);
    //     elements_multiline_text_aligned(canvas, 64, 2, AlignCenter, AlignTop, "GPIO Output Mode Test");
    //     canvas_set_font(canvas, FontSecondary);
    //     elements_multiline_text_aligned(
    //         canvas, 64, 16, AlignCenter, AlignTop, "Press < or > to change pin");
    //     elements_multiline_text_aligned(
    //         canvas,
    //         64,
    //         32,
    //         AlignCenter,
    //         AlignTop,
    //         gpio_items_get_pin_name(model->gpio_items, model->pin_idx));
}

static bool sensor_IRCam_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    SensorIRCam* sensor_IRCam = (SensorIRCam*)context;
    bool consumed = false;

    if(event->type == InputTypeShort) {
        if(event->key == InputKeyRight) {
            consumed = sensor_IRCam_process_right(sensor_IRCam);
        } else if(event->key == InputKeyLeft) {
            consumed = sensor_IRCam_process_left(sensor_IRCam);
        }
    } else if(event->key == InputKeyOk) {
        consumed = sensor_IRCam_process_ok(sensor_IRCam, event);
    }

    return consumed;
}

static bool sensor_IRCam_process_left(SensorIRCam* sensor_IRCam) {
    //TODO
    return true;
}

static bool sensor_IRCam_process_right(SensorIRCam* sensor_IRCam) {
    //TODO
    return true;
}

static bool sensor_IRCam_process_ok(SensorIRCam* sensor_IRCam, InputEvent* event) {
    //TODO
    bool consumed = true;
    return consumed;
}

SensorIRCam* sensor_IRCam_alloc(SensorApp* app) {
    SensorIRCam* sensor_IRCam = malloc(sizeof(SensorIRCam));

    UNUSED(app);
    sensor_IRCam->view = view_alloc();
    //view_allocate_model(sensor_IRCam->view, ViewModelTypeLocking, sizeof(GpioTestModel));
    //TODO figure this out

    //with_view_model(
    //    sensor_IRCam->view, GpioTestModel * model, { model->gpio_items = gpio_items; }, false);
    //TODO figure this out

    view_set_context(sensor_IRCam->view, sensor_IRCam);
    view_set_draw_callback(sensor_IRCam->view, sensor_IRCam_draw_callback);
    view_set_input_callback(sensor_IRCam->view, sensor_IRCam_input_callback);

    return sensor_IRCam;
}

void sensor_IRCam_free(SensorIRCam* sensor_IRCam) {
    furi_assert(sensor_IRCam);
    view_free(sensor_IRCam->view);
    //TODO free the rest
    free(sensor_IRCam);
}

View* sensor_IRCam_get_view(SensorIRCam* sensor_IRCam) {
    furi_assert(sensor_IRCam);
    return sensor_IRCam->view;
}

void sensor_IRCam_set_ok_callback(
    SensorIRCam* sensor_IRCam,
    GpioTestOkCallback callback,
    void* context) {
    furi_assert(sensor_IRCam);
    furi_assert(callback);
    // with_view_model(
    //     sensor_IRCam->view,
    //     GpioTestModel * model,
    //     {
    //         UNUSED(model);
    //         sensor_IRCam->callback = callback;
    //         sensor_IRCam->context = context;
    //     },
    //     false);
    //TODO fix / investigate
}
