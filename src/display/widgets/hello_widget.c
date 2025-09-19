#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(hello_widget, CONFIG_ZMK_LOG_LEVEL);

#include <lvgl.h>
#include <zmk/display/widgets.h>   // Widget helper macros
#include <zmk/event_manager.h>
#include <zmk/events/position_state_changed.h> // example event (keypress)

struct hello_widget {
    lv_obj_t *container;
    lv_obj_t *label;
    int frame;
    struct k_work_delayable tick_work;
};

static void tick_handler(struct k_work *work);

static void hello_set_text(struct hello_widget *w) {
    // Simple 2-frame "animation"
    const char *frames[] = {"HELLO", "HELLO."};
    lv_label_set_text(w->label, frames[w->frame & 1]);
}

static void tick_handler(struct k_work *work) {
    struct k_work_delayable *dwork = CONTAINER_OF(work, struct k_work_delayable, work);
    struct hello_widget *w = CONTAINER_OF(dwork, struct hello_widget, tick_work);
    w->frame++;
    hello_set_text(w);
    k_work_reschedule(&w->tick_work, K_MSEC(500)); // 2 fps
}

int zmk_widget_hello_init(struct hello_widget *w, lv_obj_t *parent) {
    w->container = lv_obj_create(parent);
    lv_obj_remove_style_all(w->container); // make it transparent
    lv_obj_set_size(w->container, 64, 16); // enough room for text

    w->label = lv_label_create(w->container);
    lv_label_set_text(w->label, "HELLO");  // initial
    lv_obj_center(w->label);

    w->frame = 0;
    k_work_init_delayable(&w->tick_work, tick_handler);
    k_work_schedule(&w->tick_work, K_MSEC(500));
    return 0;
}

lv_obj_t *zmk_widget_hello_obj(struct hello_widget *w) {
    return w->container;
}

// --- Optional: react to keypresses to "ping" the animation ---
static int hello_keypress_listener(const struct zmk_position_state_changed *ev) {
    ARG_UNUSED(ev);
    // Nudge the animation when any key is pressed
    // (Could update a counter, WPM, etc.)
    return 0;
}

ZMK_LISTENER(hello_widget, hello_keypress_listener);
ZMK_SUBSCRIPTION(hello_widget, zmk_position_state_changed);
