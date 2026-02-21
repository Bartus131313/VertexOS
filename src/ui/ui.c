#include "ui/ui.h"

void ui_print_at(int x, int y, const char* str, uint8_t color) {
    uint16_t* video_mem = (uint16_t*)0xB8000;
    for (int i = 0; str[i] != '\0'; i++) {
        video_mem[y * 80 + x + i] = (uint16_t)str[i] | (uint16_t)color << 8;
    }
}

void ui_draw_window(Window* win) {
    uint16_t* video_mem = (uint16_t*)0xB8000;
    uint8_t color = win->border_color;

    // Draw Corners
    video_mem[win->y * 80 + win->x] = (uint16_t)0xDA | (uint16_t)color << 8;
    video_mem[win->y * 80 + win->x + win->width - 1] = (uint16_t)0xBF | (uint16_t)color << 8;
    video_mem[(win->y + win->height - 1) * 80 + win->x] = (uint16_t)0xC0 | (uint16_t)color << 8;
    video_mem[(win->y + win->height - 1) * 80 + win->x + win->width - 1] = (uint16_t)0xD9 | (uint16_t)color << 8;

    // Draw Horizontal lines
    for (int i = 1; i < win->width - 1; i++) {
        video_mem[win->y * 80 + win->x + i] = (uint16_t)0xC4 | (uint16_t)color << 8;
        video_mem[(win->y + win->height - 1) * 80 + win->x + i] = (uint16_t)0xC4 | (uint16_t)color << 8;
    }

    // Draw Vertical lines
    for (int i = 1; i < win->height - 1; i++) {
        video_mem[(win->y + i) * 80 + win->x] = (uint16_t)0xB3 | (uint16_t)color << 8;
        video_mem[(win->y + i) * 80 + win->x + win->width - 1] = (uint16_t)0xB3 | (uint16_t)color << 8;
    }

    // Print Title
    if (win->title) {
        ui_print_at(win->x + 2, win->y, win->title, win->title_color);
    }
}

void ui_draw_statusbar(const char* mode, const char* system_name) {
    uint16_t* video_mem = (uint16_t*)0xB8000;
    uint8_t bar_color = 0x70; // Black text on Gray background
    
    // Fill the 24th row with the bar color
    for (int x = 0; x < 80; x++) {
        video_mem[24 * 80 + x] = (uint16_t)' ' | (uint16_t)bar_color << 8;
    }

    // Print the Mode (e.g., -- NORMAL --)
    ui_print_at(1, 24, mode, bar_color);
    
    // Print System Name in the middle
    ui_print_at(35, 24, system_name, bar_color);
    
    // The time will be updated here later
}

void ui_draw_line_numbers() {
    for (int y = 0; y < 24; y++) {
        char buf[4];
        // Simple logic to convert y to string
        buf[0] = (y / 10) + '0';
        buf[1] = (y % 10) + '0';
        buf[2] = ' ';
        buf[3] = '\0';
        ui_print_at(0, y, buf, 0x08); // Dark gray for line numbers
    }
}