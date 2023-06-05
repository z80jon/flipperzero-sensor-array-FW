#include "sensor_test.h"
#include "../sensor_items.h"

#include <gui/elements.h>

struct GpioTest {
    View* view;
    GpioTestOkCallback callback;
    void* context;
};

typedef struct {
    uint8_t pin_idx;
    GPIOItems* gpio_items;
} GpioTestModel;

static bool sensor_IRCam_process_left(GpioTest* sensor_IRCam);
static bool sensor_IRCam_process_right(GpioTest* sensor_IRCam);
static bool sensor_IRCam_process_ok(GpioTest* sensor_IRCam, InputEvent* event);

static void sensor_IRCam_draw_callback(Canvas* canvas, void* _model) {
    GpioTestModel* model = _model;
    canvas_set_font(canvas, FontPrimary);
    elements_multiline_text_aligned(canvas, 64, 2, AlignCenter, AlignTop, "GPIO Output Mode Test");
    canvas_set_font(canvas, FontSecondary);
    elements_multiline_text_aligned(
        canvas, 64, 16, AlignCenter, AlignTop, "Press < or > to change pin");
    elements_multiline_text_aligned(
        canvas,
        64,
        32,
        AlignCenter,
        AlignTop,
        gpio_items_get_pin_name(model->gpio_items, model->pin_idx));
}

static bool sensor_IRCam_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    GpioTest* sensor_IRCam = context;
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

static bool sensor_IRCam_process_left(GpioTest* sensor_IRCam) {
    with_view_model(
        sensor_IRCam->view,
        GpioTestModel * model,
        {
            if(model->pin_idx) {
                model->pin_idx--;
            }
        },
        true);
    return true;
}

static bool sensor_IRCam_process_right(GpioTest* sensor_IRCam) {
    with_view_model(
        sensor_IRCam->view,
        GpioTestModel * model,
        {
            if(model->pin_idx < gpio_items_get_count(model->gpio_items)) {
                model->pin_idx++;
            }
        },
        true);
    return true;
}

static bool sensor_IRCam_process_ok(GpioTest* sensor_IRCam, InputEvent* event) {
    bool consumed = false;

    with_view_model(
        sensor_IRCam->view,
        GpioTestModel * model,
        {
            if(event->type == InputTypePress) {
                if(model->pin_idx < gpio_items_get_count(model->gpio_items)) {
                    gpio_items_set_pin(model->gpio_items, model->pin_idx, true);
                } else {
                    gpio_items_set_all_pins(model->gpio_items, true);
                }
                consumed = true;
            } else if(event->type == InputTypeRelease) {
                if(model->pin_idx < gpio_items_get_count(model->gpio_items)) {
                    gpio_items_set_pin(model->gpio_items, model->pin_idx, false);
                } else {
                    gpio_items_set_all_pins(model->gpio_items, false);
                }
                consumed = true;
            }
            sensor_IRCam->callback(event->type, sensor_IRCam->context);
        },
        true);

    return consumed;
}

GpioTest* sensor_IRCam_alloc(GPIOItems* gpio_items) {
    GpioTest* sensor_IRCam = malloc(sizeof(GpioTest));

    sensor_IRCam->view = view_alloc();
    view_allocate_model(sensor_IRCam->view, ViewModelTypeLocking, sizeof(GpioTestModel));

    with_view_model(
        sensor_IRCam->view, GpioTestModel * model, { model->gpio_items = gpio_items; }, false);

    view_set_context(sensor_IRCam->view, sensor_IRCam);
    view_set_draw_callback(sensor_IRCam->view, sensor_IRCam_draw_callback);
    view_set_input_callback(sensor_IRCam->view, sensor_IRCam_input_callback);

    return sensor_IRCam;
}

void sensor_IRCam_free(GpioTest* sensor_IRCam) {
    furi_assert(sensor_IRCam);
    view_free(sensor_IRCam->view);
    free(sensor_IRCam);
}

View* sensor_IRCam_get_view(GpioTest* sensor_IRCam) {
    furi_assert(sensor_IRCam);
    return sensor_IRCam->view;
}

void sensor_IRCam_set_ok_callback(
    GpioTest* sensor_IRCam,
    GpioTestOkCallback callback,
    void* context) {
    furi_assert(sensor_IRCam);
    furi_assert(callback);
    with_view_model(
        sensor_IRCam->view,
        GpioTestModel * model,
        {
            UNUSED(model);
            sensor_IRCam->callback = callback;
            sensor_IRCam->context = context;
        },
        false);
}
