#include "view_IRCam.h"
//#include "sensor_array_images.h" //Auto-generated file from images folder

#include <gui/elements.h>
#include <gui/canvas.h>
#include "sensor_array_icons.h"

static bool view_IRCam_process_left(SensorIRCam* view_IRCam);
static bool view_IRCam_process_right(SensorIRCam* view_IRCam);
static bool view_IRCam_process_ok(SensorIRCam* view_IRCam, InputEvent* event);

typedef struct {
    SensorIRCam* IRCam;
    //TODO add graphics modes, etc
} IRCamModel;

static void view_IRCam_draw_callback(Canvas* canvas, void* _model) {
    IRCamModel* model = _model;
    UNUSED(model);
    //GridEye* ge = model->IRCam->ge;
    //gridEye_update(ge);
    canvas_set_font(canvas, FontPrimary);
    elements_multiline_text_aligned(canvas, 64, 2, AlignCenter, AlignTop, "GPIO Output Mode Test");

    //Draw the 8x8 grid of pixels
    for(uint8_t y = 0; y < 8; y++) {
        for(uint8_t x = 0; x < 8; x++) {
            if((x + y) % 2 == 0) {
                canvas_draw_icon_ex(canvas, x * 8, y * 8, &I_grayscale_8x8_4, IconRotation0);
            } else {
                canvas_draw_icon_ex(canvas, x * 8, y * 8, &I_grayscale_8x8_0, IconRotation0);
            }
        }
    }
}

static bool view_IRCam_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    SensorIRCam* view_IRCam = (SensorIRCam*)context;
    bool consumed = false;

    if(event->type == InputTypeShort) {
        if(event->key == InputKeyRight) {
            consumed = view_IRCam_process_right(view_IRCam);
        } else if(event->key == InputKeyLeft) {
            consumed = view_IRCam_process_left(view_IRCam);
        }
    } else if(event->key == InputKeyOk) {
        consumed = view_IRCam_process_ok(view_IRCam, event);
    }

    return consumed;
}

static bool view_IRCam_process_left(SensorIRCam* view_IRCam) {
    with_view_model(
        view_IRCam->view, IRCamModel * model, { UNUSED(model); }, true);
    return true;
}

static bool view_IRCam_process_right(SensorIRCam* view_IRCam) {
    with_view_model(
        view_IRCam->view, IRCamModel * model, { UNUSED(model); }, true);
    return true;
}

static bool view_IRCam_process_ok(SensorIRCam* view_IRCam, InputEvent* event) {
    with_view_model(
        view_IRCam->view, IRCamModel * model, { UNUSED(model); }, true);
    bool consumed = true;
    UNUSED(event);
    return consumed;
}

SensorIRCam* view_IRCam_alloc(SensorApp* app) {
    SensorIRCam* view_IRCam = malloc(sizeof(SensorIRCam));

    UNUSED(app);
    view_IRCam->view = view_alloc();
    view_allocate_model(view_IRCam->view, ViewModelTypeLocking, sizeof(IRCamModel));
    view_IRCam->ge = gridEye_init(0x00, GridEyeFrameRate_10FPS);

    with_view_model(
        view_IRCam->view, IRCamModel * model, { model->IRCam = view_IRCam; }, false);

    view_set_context(view_IRCam->view, view_IRCam);
    view_set_draw_callback(view_IRCam->view, view_IRCam_draw_callback);
    view_set_input_callback(view_IRCam->view, view_IRCam_input_callback);

    return view_IRCam;
}

void view_IRCam_free(SensorIRCam* view_IRCam) {
    furi_assert(view_IRCam);
    view_free(view_IRCam->view);
    gridEye_free(view_IRCam->ge);
    free(view_IRCam);
}

View* view_IRCam_get_view(SensorIRCam* view_IRCam) {
    furi_assert(view_IRCam);
    return view_IRCam->view;
}

void view_IRCam_set_ok_callback(
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
