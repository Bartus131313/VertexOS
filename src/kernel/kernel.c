#include "kernel/shell.h"
#include "ui/ui.h"
#include "kernel/terminal.h"

extern void init_gdt();
extern void init_idt();

void initialize() {
    init_gdt();
    init_idt();
}

// void show_welcome_ui() {
//     terminal_clear();
    
//     Window win = {
//         .x = 20, .y = 5, 
//         .width = 40, .height = 10, 
//         .title = " BARTEK OS STATUS ", 
//         .border_color = 0x0B, // Light Cyan
//         .title_color = 0x0F   // White
//     };
    
//     ui_draw_window(&win);
//     ui_print_at(22, 7, "CPU: AuthenticAMD", 0x07);
//     ui_print_at(22, 8, "RAM: 128 MB", 0x07);
//     ui_print_at(22, 12, "Press any key to start...", 0x8F); // 0x8F makes it blink!
// }

typedef struct {
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    // ... there are more fields, but we only need these for now
} multiboot_info_t;

void kmain(multiboot_info_t* mbi) {
    initialize();
    terminal_clear();
    
    // Draw the "Editor" UI
    ui_draw_statusbar(" TERMINAL ", " Omega OS v1.0.0 ");
    
    kprint("CarbonOS Kernel Version 0.1.0-alpha\n");
    kprint("Copyright (c) 2026 Bartek. All rights reserved.\n");
    kprint("----------------------------------------------\n");

    uint32_t ram_kb = mbi->mem_lower + mbi->mem_upper;
    uint32_t ram_mb = ram_kb / 1024;
    
    kprint("Memory detected: ");
    kprint_int(ram_mb);
    kprint(" MB\n");

    shell_print_prefix();

    while(1) { asm volatile("hlt"); }
}