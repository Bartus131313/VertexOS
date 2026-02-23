#include "drivers/graphics/vesa.h"
#include "util/heap.h"

// ----------------------------------------------------------------------------
// Private State (Static ensures these aren't accessible outside this file)
// ----------------------------------------------------------------------------
static uint8_t* front_buffer = 0;  // Physical RAM address (using uint8_t for byte math)
static uint32_t* back_buffer = 0;  // Our "scratchpad" (In Kernel Heap)

static uint32_t screen_width  = 0;
static uint32_t screen_height = 0;
static uint32_t screen_pitch  = 0;

// ----------------------------------------------------------------------------
// Initialization
// ----------------------------------------------------------------------------
bool vesa_init(multiboot_info_t* mbi) {
    // Check bit 12 of flags to see if a framebuffer is available
    if (!(mbi->flags & (1 << 12))) {
        return false; // Still in text mode, abort VESA setup
    }

    front_buffer  = (uint8_t*)(uint32_t)(mbi->framebuffer_addr & 0xFFFFFFFF);
    screen_width  = mbi->framebuffer_width;
    screen_height = mbi->framebuffer_height;
    screen_pitch  = mbi->framebuffer_pitch;

    // Allocate memory for our back buffer (tightly packed, no pitch padding needed here)
    uint32_t buffer_size = screen_width * screen_height * sizeof(uint32_t);
    back_buffer = (uint32_t*)kmalloc(buffer_size);

    if (!back_buffer) {
        return false; // Heap allocation failed
    }

    return true;
}

// ----------------------------------------------------------------------------
// Getters
// ----------------------------------------------------------------------------
uint32_t vesa_get_screen_width() { return screen_width; }
uint32_t vesa_get_screen_height() { return screen_height; }
void vesa_get_screen_size(uint32_t* width, uint32_t* height) {
    if (width) *width = screen_width;
    if (height) *height = screen_height;
}

// ----------------------------------------------------------------------------
// Rendering
// ----------------------------------------------------------------------------
void vesa_flip() {
    if (!front_buffer || !back_buffer) return;

    // We must copy row-by-row because the front_buffer's pitch might be 
    // larger than width * 4 (due to hardware alignment padding).
    uint32_t bytes_per_row = screen_width * sizeof(uint32_t);

    for (uint32_t y = 0; y < screen_height; y++) {
        void* dest_row = front_buffer + (y * screen_pitch);
        void* src_row  = back_buffer + (y * screen_width);
        
        sse_memcpy(dest_row, src_row, bytes_per_row);
    }
}

void vesa_draw_pixel(int x, int y, uint32_t color) {
    // CRITICAL: Bounds checking prevents kernel panics
    if (x < 0 || x >= (int)screen_width || y < 0 || y >= (int)screen_height) {
        return; 
    }
    back_buffer[y * screen_width + x] = color;
}

void vesa_draw_rect(int x, int y, int w, int h, uint32_t color) {
    // Clip the rectangle to the screen bounds
    int start_x = (x < 0) ? 0 : x;
    int start_y = (y < 0) ? 0 : y;
    int end_x   = (x + w > (int)screen_width)  ? (int)screen_width  : x + w;
    int end_y   = (y + h > (int)screen_height) ? (int)screen_height : y + h;

    for (int i = start_y; i < end_y; i++) {
        uint32_t* row = &back_buffer[i * screen_width];
        for (int j = start_x; j < end_x; j++) {
            row[j] = color;
        }
    }
}

// ----------------------------------------------------------------------------
// UI Elements
// ----------------------------------------------------------------------------

static const uint8_t mouse_cursor[17][12] = {
    {2,2,0,0,0,0,0,0,0,0,0,0},
    {2,1,2,0,0,0,0,0,0,0,0,0},
    {2,1,1,2,0,0,0,0,0,0,0,0},
    {2,1,1,1,2,0,0,0,0,0,0,0},
    {2,1,1,1,1,2,0,0,0,0,0,0},
    {2,1,1,1,1,1,2,0,0,0,0,0},
    {2,1,1,1,1,1,1,2,0,0,0,0},
    {2,1,1,1,1,1,1,1,2,0,0,0},
    {2,1,1,1,1,1,1,1,1,2,0,0},
    {2,1,1,1,1,1,1,1,1,1,2,0},
    {2,1,1,1,1,1,2,2,2,2,2,2},
    {2,1,1,2,1,1,2,0,0,0,0,0},
    {2,1,2,0,2,1,1,2,0,0,0,0},
    {2,2,0,0,2,1,1,2,0,0,0,0},
    {0,0,0,0,0,2,1,1,2,0,0,0},
    {0,0,0,0,0,2,1,1,2,0,0,0},
    {0,0,0,0,0,0,2,2,0,0,0,0}
};

void vesa_draw_mouse(int x, int y) {
    for (int i = 0; i < 17; i++) {
        for (int j = 0; j < 12; j++) {
            if (mouse_cursor[i][j] == 1) {
                vesa_draw_pixel(x + j, y + i, RGB(255, 255, 255));
            } else if (mouse_cursor[i][j] == 2) {
                vesa_draw_pixel(x + j, y + i, RGB(0, 0, 0));
            }
        }
    }
}