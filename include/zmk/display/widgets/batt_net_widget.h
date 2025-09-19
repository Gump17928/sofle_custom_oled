#pragma once
#include <lvgl.h>

struct batt_net_widget;

int  zmk_widget_batt_net_init(struct batt_net_widget *widget, lv_obj_t *parent);
lv_obj_t *zmk_widget_batt_net_obj(struct batt_net_widget *widget);
