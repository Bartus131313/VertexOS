#ifndef UI_H
#define UI_H

#include <stdint.h>
#include "kernel/terminal.h"

typedef struct {
    int x, y, width, height;
    char* title;
    uint8_t border_color;
    uint8_t title_color;
} Window;

void ui_init();

void ui_draw_window(Window* win);
void ui_draw_rect(int x, int y, int w, int h, uint8_t color);
void ui_print_at(int x, int y, const char* str, uint8_t color);
void ui_draw_statusbar(const char* mode, const char* system_name);
void ui_draw_line_numbers();
#endif