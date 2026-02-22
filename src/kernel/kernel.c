#include "kernel/shell.h"
#include "kernel/terminal.h"
#include "kernel/multiboot.h"
#include "drivers/graphics/vesa.h"
#include "drivers/mouse.h"
#include "ui/ui.h"
#include "util/memory.h"
#include "util/heap.h"

// Define the global variable here (without extern)
multiboot_info_t* global_mbi = 0;

// Import global functions from ASM
extern void init_gdt();
extern void init_idt();

void enable_sse() {
    asm volatile (
        "mov %%cr0, %%eax\n"
        "and $0xFFFB, %%ax\n"      // Clear CR0.EM (bit 2)
        "or $0x2, %%ax\n"          // Set CR0.MP (bit 1)
        "mov %%eax, %%cr0\n"
        "mov %%cr4, %%eax\n"
        "or $0x600, %%ax\n"        // Set CR4.OSFXSR (bit 9) and CR4.OSXMMEXCPT (bit 10)
        "mov %%eax, %%cr4\n"
        ::: "eax"
    );
}

// Initialize critical things
void initialize() {
    // Initialize GDT & IDT
    init_gdt();
    init_idt();

    enable_sse();

    // Initialize Physical Memory Manager for future memory operations
    pmm_init(global_mbi);

    // Initialize Heap
    kheap_init();

    // Initialize VESA
    vesa_init(global_mbi);

    mouse_init();
}

// DEPRECATED: Used to run in Text Mode
/*
void kmain(multiboot_info_t* mbi_ptr) {
    // Save the pointer passed by the bootloader into our global variable
    global_mbi = mbi_ptr;

    // Call initialization function
    initialize();

    // Clear terminal at start
    terminal_clear();

    // Draw starter UI (bottom bar)
    ui_init();
    
    // Print welcome message
    kprint("Welcome to %s Kernel!\n", SYSTEM_NAME);
    shell_print_prefix();

    // Halt the CPU forever
    while(1) { asm volatile("hlt"); }
}
    */

// Tests for screen colors

void test_gradient(uint32_t width, uint32_t height) {
    for (uint32_t y = 0; y < height; y++) {
        for (uint32_t x = 0; x < width; x++) {
            // Map X to Red (0-255) and Y to Green (0-255)
            uint8_t r = (x * 255) / width;
            uint8_t g = (y * 255) / height;
            uint8_t b = 128; // Constant blue

            vesa_draw_pixel(x, y, RGB(r, g, b));
        }
    }
}

void test_desktop(uint32_t width, uint32_t height) {
    // 1. Draw a "Deep Space" background gradient
    for (uint32_t y = 0; y < height; y++) {
        uint8_t color_val = (y * 100) / height; // 0 to 100 range
        vesa_draw_rect(0, y, width, 1, RGB(20 + (color_val / 2), 30 + color_val, 50 + color_val));
    }

    // 2. Taskbar (Dark translucent style)
    vesa_draw_rect(0, height - 45, width, 45, RGB(15, 15, 20));

    // 3. Start Button (Accent color)
    vesa_draw_rect(5, height - 40, 80, 35, RGB(0, 120, 215)); 

    // 4. Draw a "Window" outline
    vesa_draw_rect(100, 100, 400, 300, RGB(240, 240, 240)); // Window body
    vesa_draw_rect(100, 100, 400, 30, RGB(0, 120, 215));    // Title bar
}

// You'll need a simple random seed or just use x * y
void test_noise(uint32_t width, uint32_t height) {
    static uint32_t seed = 0x12345678;
    for (uint32_t x = 0; x < width; x++) {
        for (uint32_t y = 0; y < height; y++) {
            seed = seed * 1103515245 + 12345;
            vesa_draw_pixel(x, y, seed);
        }
    }
}

// void kmain(multiboot_info_t* mbi_ptr) {
//     global_mbi = mbi_ptr;
//     initialize();

//     uint32_t w, h;
//     vesa_get_screen_size(&w, &h);

//     // CHOOSE ONE:
//     // test_desktop(w, h);
//     test_gradient(w, h);
//     // test_noise(w, h);

//     vesa_flip();

//     while(1) { asm volatile("hlt"); }
// }

void kmain(multiboot_info_t* mbi_ptr) {
    global_mbi = mbi_ptr;
    initialize();

    // Center the mouse to start
    mouse_x = vesa_screen_width / 2;
    mouse_y = vesa_screen_height / 2;
    
    // Enable interrupts so the mouse starts talking
    asm volatile("sti");

    while(1) { 
        // Draw Background (Clears the old mouse position)
        vesa_draw_rect(0, 0, vesa_screen_width, vesa_screen_height, RGB(50, 100, 150));

        // Draw Taskbar
        // vesa_draw_rect(0, vesa_screen_height - 40, vesa_screen_width, 40, RGB(200, 200, 200));

        // test_gradient(vesa_screen_width, vesa_screen_height);

        // Define the Start Button area
        int start_btn_x = 15;
        int start_btn_y = 15;
        int start_btn_w = 150;
        int start_btn_h = 50;
            
        // Inside while(1)...
        bool is_over_start = (mouse_x >= start_btn_x && mouse_x <= start_btn_x + start_btn_w) &&
                             (mouse_y >= start_btn_y && mouse_y <= start_btn_y + start_btn_h);
            
        if (is_over_start && mouse_left_click) {
            // Button is pressed! Draw it "pushed down" (darker)
            vesa_draw_rect(start_btn_x, start_btn_y, start_btn_w, start_btn_h, RGB(100, 100, 100));
        } else if (is_over_start) {
            // Hovering! Draw it slightly lighter
            vesa_draw_rect(start_btn_x, start_btn_y, start_btn_w, start_btn_h, RGB(180, 180, 180));
        } else {
            // Normal state
            vesa_draw_rect(start_btn_x, start_btn_y, start_btn_w, start_btn_h, RGB(150, 150, 150));
        }

        // Draw the Mouse
        vesa_draw_mouse(mouse_x, mouse_y);

        // Flip the back-buffer to the screen
        vesa_flip(); 

        // Halt CPU until next interrupt to save power, 
        // instead of burning 100% CPU spinning the loop.
        asm volatile("hlt");
    }
}