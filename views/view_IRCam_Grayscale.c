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
    IconRotation rotation);

typedef struct {
    SensorIRCam* IRCam;
    //TODO add graphics modes, etc
} IRCamModel;

static void view_IRCam_grayscale_draw_callback(Canvas* canvas, void* _model) {
    IRCamModel* model = _model;
    furi_check(model->IRCam->ge != NULL);
    GridEye* ge = model->IRCam->ge;
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 75, 8, "AMG8833");
    canvas_draw_str(canvas, 14, 33, "Working...");

    FuriString* buff = furi_string_alloc();

    furi_string_printf(buff, "Max: %.1FC", (double)ge->max);
    canvas_draw_str(canvas, 75, 36, furi_string_get_cstr(buff));
    furi_string_printf(buff, "Min: %.1FC", (double)ge->min);
    canvas_draw_str(canvas, 75, 26, furi_string_get_cstr(buff));

    furi_string_free(buff);

    //Draw the 8x8 grid of pixels
    for(uint8_t y = 0; y < 8; y++) {
        for(uint8_t x = 0; x < 8; x++) {
            grayscale_render(
                canvas,
                56 - x * 8,
                56 - y * 8,
                gridEye_getTemperatureGrayscale(ge, y * 8 + x),
                IconRotation0);
        }
    }
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
    } else if(event->key == InputKeyOk) {
        consumed = view_IRCam_grayscale_process_ok(view_IRCam, event);
    }

    return consumed;
}

static bool view_IRCam_grayscale_process_left(SensorIRCam* view_IRCam) {
    with_view_model(
        view_IRCam->view, IRCamModel * model, { UNUSED(model); }, true);
    return true;
}

static bool view_IRCam_grayscale_process_right(SensorIRCam* view_IRCam) {
    with_view_model(
        view_IRCam->view, IRCamModel * model, { UNUSED(model); }, true);
    return true;
}

static bool view_IRCam_grayscale_process_ok(SensorIRCam* view_IRCam, InputEvent* event) {
    with_view_model(
        view_IRCam->view, IRCamModel * model, { UNUSED(model); }, true);
    bool consumed = true;
    UNUSED(event);
    return consumed;
}

SensorIRCam* view_IRCam_grayscale_alloc(SensorApp* app) {
    SensorIRCam* view_IRCam = malloc(sizeof(SensorIRCam));

    UNUSED(app);
    view_IRCam->view = view_alloc();
    view_allocate_model(view_IRCam->view, ViewModelTypeLocking, sizeof(IRCamModel));
    view_IRCam->ge = NULL;

    with_view_model(
        view_IRCam->view, IRCamModel * model, { model->IRCam = view_IRCam; }, false);

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
        IRCamModel * model,
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
    IconRotation rotation) {
    //TODO change to pound define, unify with grideye.c binning algorithm pound define
    furi_assert(grayscale_value < 9);
    switch(grayscale_value) {
    case 0:
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
        canvas_draw_icon_ex(canvas, x, y, &I_grayscale_8x8_8, rotation);
        break;
    }
}