#include "view_IRCam_grayscale.h"
//#include "sensor_array_images.h" //Auto-generated file from images folder

#include <gui/elements.h>
#include <gui/canvas.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "string.h"

#include "sensor_array_icons.h"

static bool view_IRCam_grayscale_process_left(SensorIRCam* view_IRCam);
static bool view_IRCam_grayscale_process_right(SensorIRCam* view_IRCam);
static bool view_IRCam_grayscale_process_ok(SensorIRCam* view_IRCam, InputEvent* event);
static void grayscale_render(
    Canvas* canvas,
    uint8_t x,
    uint8_t y,
    uint8_t grayscale_value,
    IconRotation rotation,
    bool invert);

typedef struct {
    SensorIRCam* IRCam;
    bool invert;
    //TODO add graphics modes, etc
} IRCam_grayscale_model;

static void view_IRCam_grayscale_draw_callback(Canvas* canvas, void* _model) {
    IRCam_grayscale_model* model = _model;
    furi_check(model->IRCam->ge != NULL);
    GridEye* ge = model->IRCam->ge;
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(canvas, 96, 1, AlignCenter, AlignTop, "Thermal Cam");
    canvas_draw_line(canvas, 65, 0, 65, 63);
    canvas_draw_line(canvas, 65, 10, 127, 10);

    FuriString* buff = furi_string_alloc();

    furi_string_printf(buff, "Min: %.1FC", (double)ge->min);
    canvas_draw_str(canvas, 75, 19, furi_string_get_cstr(buff));
    furi_string_printf(buff, "Max: %.1FC", (double)ge->max);
    canvas_draw_str(canvas, 75, 28, furi_string_get_cstr(buff));

    //Grab the center 4 pixels
    float center_avg = (gridEye_getTemperature(ge, 37) + gridEye_getTemperature(ge, 36) +
                        gridEye_getTemperature(ge, 29) + gridEye_getTemperature(ge, 28)) /
                       4;
    furi_string_printf(buff, "Cntr: %.1FC", (double)center_avg);
    canvas_draw_str(canvas, 75, 37, furi_string_get_cstr(buff));

    furi_string_free(buff);

    //Draw the 8x8 grid of pixels
    for(uint8_t y = 0; y < 8; y++) {
        for(uint8_t x = 0; x < 8; x++) {
            grayscale_render(
                canvas,
                56 - x * 8,
                56 - y * 8,
                gridEye_getTemperatureGrayscale(ge, y * 8 + x),
                IconRotation0,
                model->invert);
        }
    }

    //Grayscale inversion status
    if(model->invert) {
        canvas_draw_str_aligned(canvas, 96, 40, AlignCenter, AlignTop, "White Hot");
    } else {
        canvas_draw_str_aligned(canvas, 96, 40, AlignCenter, AlignTop, "Black Hot");
    }

    // Button
    canvas_draw_rbox(canvas, 73, 50, 46, 13, 3);
    canvas_set_color(canvas, ColorWhite);
    canvas_draw_icon(canvas, 78, 52, &I_button_ok_9x9);
    canvas_draw_str_aligned(canvas, 90, 53, AlignLeft, AlignTop, "Invert");
}

static bool view_IRCam_grayscale_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    SensorIRCam* view_IRCam = (SensorIRCam*)context;
    bool consumed = false;

    if(event->type == InputTypeShort) {
        if(event->key == InputKeyRight) {
            consumed = view_IRCam_grayscale_process_right(view_IRCam);
        } else if(event->key == InputKeyLeft) {
            consumed = view_IRCam_grayscale_process_left(view_IRCam);
        }
    } else if(event->key == InputKeyOk && event->type == InputTypePress) {
        consumed = view_IRCam_grayscale_process_ok(view_IRCam, event);
    }

    return consumed;
}

static bool view_IRCam_grayscale_process_left(SensorIRCam* view_IRCam) {
    with_view_model(
        view_IRCam->view, IRCam_grayscale_model * model, { UNUSED(model); }, true);
    return true;
}

static bool view_IRCam_grayscale_process_right(SensorIRCam* view_IRCam) {
    with_view_model(
        view_IRCam->view, IRCam_grayscale_model * model, { UNUSED(model); }, true);
    return true;
}

static bool view_IRCam_grayscale_process_ok(SensorIRCam* view_IRCam, InputEvent* event) {
    with_view_model(
        view_IRCam->view, IRCam_grayscale_model * model, { model->invert = !model->invert; }, true);
    bool consumed = true;
    UNUSED(event);
    return consumed;
}

SensorIRCam* view_IRCam_grayscale_alloc(SensorApp* app) {
    SensorIRCam* view_IRCam = malloc(sizeof(SensorIRCam));

    UNUSED(app);
    view_IRCam->view = view_alloc();
    view_allocate_model(view_IRCam->view, ViewModelTypeLocking, sizeof(IRCam_grayscale_model));
    view_IRCam->ge = NULL;

    with_view_model(
        view_IRCam->view,
        IRCam_grayscale_model * model,
        {
            model->IRCam = view_IRCam;
            model->invert = false;
        },
        false);

    view_set_context(view_IRCam->view, view_IRCam);
    view_set_draw_callback(view_IRCam->view, view_IRCam_grayscale_draw_callback);
    view_set_input_callback(view_IRCam->view, view_IRCam_grayscale_input_callback);

    return view_IRCam;
}

void view_IRCam_grayscale_free(SensorIRCam* view_IRCam) {
    furi_assert(view_IRCam);
    view_free(view_IRCam->view);
    if(view_IRCam->ge != NULL) gridEye_free(view_IRCam->ge);
    free(view_IRCam);
}

View* view_IRCam_grayscale_get_view(SensorIRCam* view_IRCam) {
    furi_assert(view_IRCam);
    return view_IRCam->view;
}

void view_IRCam_grayscale_set_ok_callback(
    SensorIRCam* view_IRCam,
    GpioTestOkCallback callback,
    void* context) {
    furi_assert(view_IRCam);
    furi_assert(callback);
    with_view_model(
        view_IRCam->view,
        IRCam_grayscale_model * model,
        {
            UNUSED(model);
            //view_IRCam->callback = callback;
            //view_IRCam->context = context;
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
    //TODO change to pound define, unify with grideye.c binning algorithm pound define
    furi_assert(grayscale_value < 9);

    if(invert) {
        grayscale_value = 8 - grayscale_value;
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