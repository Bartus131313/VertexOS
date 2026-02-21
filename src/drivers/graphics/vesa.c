#include "drivers/graphics/vesa.h"

uint32_t* front_buffer; // The actual screen (Physical RAM)
uint32_t* back_buffer;  // Our "scratchpad" (In Kernel Heap)
uint32_t screen_width, screen_height, screen_pitch;

void vesa_init(multiboot_info_t* mbi) {
    // Check bit 12 of flags to see if a framebuffer is available
    if (!(mbi->flags & (1 << 12))) {
        // If we reach here, we are STILL in text mode.
        // We need to stop or the memory writes will corrupt the kernel.
        return; 
    }

    front_buffer = (uint32_t*)(uint32_t)(mbi->framebuffer_addr & 0xFFFFFFFF);
    screen_width = mbi->framebuffer_width;
    screen_height = mbi->framebuffer_height;
    screen_pitch = mbi->framebuffer_pitch;

    // Allocate memory for our back buffer so we don't flicker
    // width * height * 4 bytes per pixel
    back_buffer = (uint32_t*)kmalloc(screen_width * screen_height * 4);
}

uint32_t vesa_get_screen_width() { return screen_width; }
uint32_t vesa_get_screen_height() { return screen_height; }
void vesa_get_screen_size(uint32_t* width, uint32_t* height) {
    *width = screen_width;
    *height = screen_height;
}

// Push the back buffer to the actual screen
void vesa_flip() {
    memcpy(front_buffer, back_buffer, screen_width * screen_height * 4);
}

void vesa_draw_pixel(int x, int y, uint32_t color) {
    back_buffer[y * screen_width + x] = color;
}

void vesa_draw_rect(int x, int y, int w, int h, uint32_t color) {
    for (int i = 0; i < h; i++) {
        // Calculate the starting position of the row
        uint32_t* dest = &back_buffer[(y + i) * screen_width + x];
        
        // Fill the entire row width with the color
        // This is much faster than calling draw_pixel in a nested loop
        for (int j = 0; j < w; j++) {
            dest[j] = color;
        }
    }
}