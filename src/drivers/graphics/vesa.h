/**
 *  VESA Graphics
 * 
 *  Linear Framebuffer (LFB) driver for VESA graphics.
 *  Handles double-buffering and basic 2D drawing primitives.
 */

#ifndef VERTEX_DRIVERS_VESA_H
#define VERTEX_DRIVERS_VESA_H

#include <stdint.h>
#include <stdbool.h>
#include "kernel/multiboot.h"

// Combines R, G, B (0-255) into a 32-bit pixel value (ARGB format)
#define RGB(r, g, b) (uint32_t)(0xFF000000 | (((uint8_t)(r)) << 16) | (((uint8_t)(g)) << 8) | ((uint8_t)(b)))

// External fast copy routine (implemented in assembly)
extern void sse_memcpy(void* dest, void* src, uint32_t n);

// Initialization
bool vesa_init(multiboot_info_t* mbi);

// Screen Info Getters
uint32_t vesa_get_screen_width();
uint32_t vesa_get_screen_height();
void vesa_get_screen_size(uint32_t* width, uint32_t* height);

// Core Graphics Operations
void vesa_flip();
void vesa_draw_pixel(int x, int y, uint32_t color);
void vesa_draw_rect(int x, int y, int w, int h, uint32_t color);
void vesa_draw_mouse(int x, int y);

#endif // VERTEX_DRIVERS_VESA_H