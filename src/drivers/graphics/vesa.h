#ifndef VESA_H
#define VESA_H

#include "kernel/multiboot.h"
#include "util/heap.h"
#include "util/memory.h"

// Combines R, G, B (0-255) into a 32-bit pixel value
#define RGB(r, g, b) (uint32_t)((((uint8_t)(r)) << 16) | (((uint8_t)(g)) << 8) | ((uint8_t)(b)))

extern uint32_t vesa_screen_width;
extern uint32_t vesa_screen_height;
extern uint32_t vesa_screen_pitch;

void vesa_init(multiboot_info_t* mbi);

// Getters
uint32_t vesa_get_screen_width();
uint32_t vesa_get_screen_height();
void vesa_get_screen_size(uint32_t* width, uint32_t* height);

// Push the back buffer to the actual screen
void vesa_flip();

void vesa_draw_pixel(int x, int y, uint32_t color);
void vesa_draw_rect(int x, int y, int w, int h, uint32_t color);
void vesa_draw_mouse(int x, int y);

#endif