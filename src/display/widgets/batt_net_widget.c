#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(batt_net_widget, CONFIG_ZMK_LOG_LEVEL);

#include <lvgl.h>
#include <stdio.h>
#include <zmk/display/widgets.h>

#include <zmk/battery.h>   // zmk_battery_state_of_charge(), zmk_battery_is_charging()
#include <zmk/usb.h>       // zmk_usb_is_powered()

#ifndef LV_SYMBOL_CHARGE   // Fallback if symbols are missing in your LVGL build
#define LV_SYMBOL_CHARGE  "⚡"
#endif
#ifndef LV_SYMBOL_USB
#define LV_SYMBOL_USB     "U"   // simple fallback
#endif
#ifndef LV_SYMBOL_BLUETOOTH
#define LV_SYMBOL_BLUETOOTH "B" // simple fallback
#endif

struct batt_net_widget {
    lv_obj_t *container;
    lv_obj_t *label;
    struct k_work_delayable poll_work;
    int32_t last_soc;
    bool last_usb;
    bool last_chg;
};

static void poll_handler(struct k_work *work);

static void set_text(struct batt_net_widget *w, int32_t soc, bool usb, bool charging) {
    // Compose: "<USB|BT> NN% [⚡]"
    // Try to use LVGL symbols if present
    const char *net = usb ? LV_SYMBOL_USB : LV_SYMBOL_BLUETOOTH;
    char buf[24];

    if (soc < 0) {
        // Battery not supported -> just show net mode
        snprintf(buf, sizeof(buf), "%s --", net);
    } else if (charging) {
        snprintf(buf, sizeof(buf), "%s %ld%% " LV_SYMBOL_CHARGE, net, (long)soc);
    } else {
        snprintf(buf, sizeof(buf), "%s %ld%%", net, (long)soc);
    }

    lv_label_set_text(w->label, buf);
}

static void poll_handler(struct k_work *work) {
    struct k_work_delayable *dwork = CONTAINER_OF(work, struct k_work_delayable, work);
    struct batt_net_widget *w = CONTAINER_OF(dwork, struct batt_net_widget, poll_work);

    int32_t soc = zmk_battery_state_of_charge();   // -ENOTSUP (<0) if not available
    bool usb     = zmk_usb_is_powered();
    bool chg     = zmk_battery_is_charging();

    // Update only if something changed (saves draw calls)
    if (soc != w->last_soc || usb != w->last_usb || chg != w->last_chg) {
        set_text(w, soc, usb, chg);
        w->last_soc = soc;
        w->last_usb = usb;
        w->last_chg = chg;
    }

    k_work_reschedule(&w->poll_work, K_SECONDS(2)); // adjust refresh rate to taste
}

int zmk_widget_batt_net_init(struct batt_net_widget *w, lv_obj_t *parent) {
    w->container = lv_obj_create(parent);
    lv_obj_remove_style_all(w->container);     // transparent
    lv_obj_set_size(w->container, 64, 16);     // fits "USB 100% ⚡"

    w->label = lv_label_create(w->container);
    lv_label_set_text(w->label, "—");
    lv_obj_center(w->label);

    w->last_soc = -999;
    w->last_usb = false;
    w->last_chg = false;

    k_work_init_delayable(&w->poll_work, poll_handler);
    k_work_schedule(&w->poll_work, K_MSEC(100)); // first poll
    return 0;
}

lv_obj_t *zmk_widget_batt_net_obj(struct batt_net_widget *w) {
    return w->container;
}
