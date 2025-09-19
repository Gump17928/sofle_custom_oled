#pragma once
#include <lvgl.h>

struct hello_widget;

int zmk_widget_hello_init(struct hello_widget *widget, lv_obj_t *parent);
lv_obj_t *zmk_widget_hello_obj(struct hello_widget *widget);
