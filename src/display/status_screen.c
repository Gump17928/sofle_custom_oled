#include <lvgl.h>
#include <zmk/display/status_screen.h>

// #include <zmk/display/widgets/hello_widget.h>
#include <zmk/display/widgets/batt_net_widget.h>

// static struct hello_widget hello;
static struct batt_net_widget battnet;

lv_obj_t *zmk_display_status_screen(void) {
    lv_obj_t *screen = lv_obj_create(NULL);
    lv_obj_remove_style_all(screen);

    // // init + place our custom widget
    // zmk_widget_hello_init(&hello, screen);
    // lv_obj_align(zmk_widget_hello_obj(&hello), LV_ALIGN_BOTTOM_LEFT, 0, 0);

    zmk_widget_batt_net_init(&battnet, screen);
    // bottom-right corner; tweak offsets for your layout
    lv_obj_align(zmk_widget_batt_net_obj(&battnet), LV_ALIGN_BOTTOM_RIGHT, 0, 0);

    // You can also include existing nice_oled widgets here, e.g., battery, layer, etc.
    // (init them, then lv_obj_align / lv_obj_set_pos)

    return screen;
}