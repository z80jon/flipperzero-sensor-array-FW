#include "view_TOFDepth_grayscale.h"
#include "../../lib/vl53l5cx/vl53l5cx_api.h"
//#include "sensor_array_images.h" //Auto-generated file from images folder

#include <gui/elements.h>
#include <gui/canvas.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "string.h"

#include "sensor_array_icons.h"

static bool view_TOFDepth_grayscale_process_left(Sensor_TOFDepth* view_TOFDepth);
static bool view_TOFDepth_grayscale_process_right(Sensor_TOFDepth* view_TOFDepth);
static bool view_TOFDepth_grayscale_process_ok(Sensor_TOFDepth* view_TOFDepth, InputEvent* event);
static void grayscale_render(
    Canvas* canvas,
    uint8_t x,
    uint8_t y,
    uint8_t grayscale_value,
    IconRotation rotation,
    bool invert);

typedef struct {
    Sensor_TOFDepth* TOFDepth;
    bool invert;
    //TODO add graphics modes, etc
} TOFDepth_grayscale_model;

static void view_TOFDepth_grayscale_draw_callback(Canvas* canvas, void* _model) {
    TOFDepth_grayscale_model* model = _model;
    furi_check(model->TOFDepth != NULL);
    TOFSensor* tof = model->TOFDepth->tof;
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(canvas, 96, 1, AlignCenter, AlignTop, "TOF Sensor");
    canvas_draw_line(canvas, 65, 0, 65, 63);
    canvas_draw_line(canvas, 65, 10, 127, 10);

    FuriString* buff = furi_string_alloc();

    furi_string_printf(buff, "Min: %dcm", tof->min / 10);
    canvas_draw_str(canvas, 75, 19, furi_string_get_cstr(buff));
    furi_string_printf(buff, "Max: %dcm", tof->max / 10);
    canvas_draw_str(canvas, 75, 28, furi_string_get_cstr(buff));

    //Grab the center 4 pixels
    float center_avg =
        (tof->TOFResultsData->distance_mm[37] + tof->TOFResultsData->distance_mm[36] +
         tof->TOFResultsData->distance_mm[29] + tof->TOFResultsData->distance_mm[28]) /
        4 / 10;
    furi_string_printf(buff, "Cntr: %.1fcm", (double)center_avg);
    canvas_draw_str(canvas, 75, 37, furi_string_get_cstr(buff));

    //furi_string_free(buff);

    //Draw the 8x8 grid of pixels
    for(uint8_t y = 0; y < 8; y++) {
        for(uint8_t x = 0; x < 8; x++) {
            grayscale_render(
                canvas, x * 8, y * 8, tof->grayscale[y * 8 + x], IconRotation0, model->invert);
        }
    }

    //Grayscale inversion status
    if(model->invert) {
        canvas_draw_str_aligned(canvas, 96, 40, AlignCenter, AlignTop, "White Far");
    } else {
        canvas_draw_str_aligned(canvas, 96, 40, AlignCenter, AlignTop, "Black Far");
    }

    // Button
    canvas_draw_rbox(canvas, 73, 50, 46, 13, 3);
    canvas_set_color(canvas, ColorWhite);
    canvas_draw_icon(canvas, 78, 52, &I_button_ok_9x9);
    canvas_draw_str_aligned(canvas, 90, 53, AlignLeft, AlignTop, "Invert");
}

static bool view_TOFDepth_grayscale_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    Sensor_TOFDepth* view_TOFDepth = (Sensor_TOFDepth*)context;
    bool consumed = false;

    if(event->type == InputTypeShort) {
        if(event->key == InputKeyRight) {
            consumed = view_TOFDepth_grayscale_process_right(view_TOFDepth);
        } else if(event->key == InputKeyLeft) {
            consumed = view_TOFDepth_grayscale_process_left(view_TOFDepth);
        }
    } else if(event->key == InputKeyOk && event->type == InputTypePress) {
        consumed = view_TOFDepth_grayscale_process_ok(view_TOFDepth, event);
    }

    return consumed;
}

static bool view_TOFDepth_grayscale_process_left(Sensor_TOFDepth* view_TOFDepth) {
    with_view_model(
        view_TOFDepth->view, TOFDepth_grayscale_model * model, { UNUSED(model); }, true);
    return true;
}

static bool view_TOFDepth_grayscale_process_right(Sensor_TOFDepth* view_TOFDepth) {
    with_view_model(
        view_TOFDepth->view, TOFDepth_grayscale_model * model, { UNUSED(model); }, true);
    return true;
}

static bool view_TOFDepth_grayscale_process_ok(Sensor_TOFDepth* view_TOFDepth, InputEvent* event) {
    with_view_model(
        view_TOFDepth->view,
        TOFDepth_grayscale_model * model,
        { model->invert = !model->invert; },
        true);
    bool consumed = true;
    UNUSED(event);
    return consumed;
}

Sensor_TOFDepth* view_TOFDepth_grayscale_alloc(SensorApp* app) {
    Sensor_TOFDepth* view_TOFDepth = (Sensor_TOFDepth*)malloc(sizeof(Sensor_TOFDepth));
    view_TOFDepth->tof = (TOFSensor*)malloc(sizeof(TOFSensor));

    view_TOFDepth->tof->TOFConfiguration =
        (VL53L5CX_Configuration*)malloc(sizeof(VL53L5CX_Configuration));
    view_TOFDepth->tof->TOFResultsData =
        (VL53L5CX_ResultsData*)malloc(sizeof(VL53L5CX_ResultsData));
    view_TOFDepth->tof->TOFConfiguration->platform.address = I2C_ADDR_VL53L5CX;

    UNUSED(app);

    view_TOFDepth->view = view_alloc();
    view_allocate_model(
        view_TOFDepth->view, ViewModelTypeLocking, sizeof(TOFDepth_grayscale_model));

    with_view_model(
        view_TOFDepth->view,
        TOFDepth_grayscale_model * model,
        {
            model->TOFDepth = view_TOFDepth;
            model->invert = false;
        },
        false);

    view_set_context(view_TOFDepth->view, view_TOFDepth);
    view_set_draw_callback(view_TOFDepth->view, view_TOFDepth_grayscale_draw_callback);
    view_set_input_callback(view_TOFDepth->view, view_TOFDepth_grayscale_input_callback);

    return view_TOFDepth;
}

void view_TOFDepth_grayscale_free(Sensor_TOFDepth* view_TOFDepth) {
    furi_assert(view_TOFDepth);
    view_free(view_TOFDepth->view);
    //if(view_TOFDepth->ge != NULL) gridEye_free(view_TOFDepth->ge);
    free(view_TOFDepth->tof->TOFConfiguration);
    free(view_TOFDepth->tof->TOFResultsData);
    free(view_TOFDepth->tof);
    free(view_TOFDepth);
}

View* view_TOFDepth_grayscale_get_view(Sensor_TOFDepth* view_TOFDepth) {
    furi_assert(view_TOFDepth);
    return view_TOFDepth->view;
}

void view_TOFDepth_grayscale_set_ok_callback(
    Sensor_TOFDepth* view_TOFDepth,
    GpioTestOkCallback callback,
    void* context) {
    furi_assert(view_TOFDepth);
    furi_assert(callback);
    with_view_model(
        view_TOFDepth->view,
        TOFDepth_grayscale_model * model,
        {
            UNUSED(model);
            //view_TOFDepth->callback = callback;
            //view_TOFDepth->context = context;
        },
        true);
    UNUSED(context);
}

static void grayscale_render(
    Canvas* canvas,
    uint8_t x,
    uint8_t y,
    uint8_t grayscale_value,
    IconRotation rotation,
    bool invert) {
    furi_assert(grayscale_value < NUM_GRAYSCALE_BINS);

    if(invert) {
        grayscale_value = NUM_GRAYSCALE_BINS - grayscale_value;
    }

    switch(grayscale_value) {
    case 0:
        canvas_set_color(canvas, ColorWhite);
        canvas_draw_box(canvas, x, y, 8, 8);
        canvas_set_color(canvas, ColorBlack);
        canvas_draw_icon_ex(canvas, x, y, &I_grayscale_8x8_0, rotation);
        break;
    case 1:
        canvas_draw_icon_ex(canvas, x, y, &I_grayscale_8x8_1, rotation);
        break;
    case 2:
        canvas_draw_icon_ex(canvas, x, y, &I_grayscale_8x8_2, rotation);
        break;
    case 3:
        canvas_draw_icon_ex(canvas, x, y, &I_grayscale_8x8_3, rotation);
        break;
    case 4:
        canvas_draw_icon_ex(canvas, x, y, &I_grayscale_8x8_4, rotation);
        break;
    case 5:
        canvas_draw_icon_ex(canvas, x, y, &I_grayscale_8x8_5, rotation);
        break;
    case 6:
        canvas_draw_icon_ex(canvas, x, y, &I_grayscale_8x8_6, rotation);
        break;
    case 7:
        canvas_draw_icon_ex(canvas, x, y, &I_grayscale_8x8_7, rotation);
        break;
    case 8:
        canvas_draw_icon_ex(canvas, x, y, &I_grayscale_8x8_8, rotation);
        break;
    default:
        //Error
        furi_check(false);
        break;
    }
}