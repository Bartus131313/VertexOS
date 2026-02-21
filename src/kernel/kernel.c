#include "kernel/shell.h"
#include "kernel/terminal.h"
#include "kernel/multiboot.h"
#include "ui/ui.h"

// Define the global variable here (without extern)
multiboot_info_t* global_mbi = 0;

// Import global functions from ASM
extern void init_gdt();
extern void init_idt();

// Initialize critical things
void initialize() {
    // Initialize GDT & IDT
    init_gdt();
    init_idt();

    // Initialize Physical Memory Manager for future memory operations
    pmm_init(global_mbi);
}

// TODO: Create cool windows
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

// Starting point of C kernel
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
    kprint("Welcome to VertexOS kernel!\n");
    shell_print_prefix();

    // Halt the CPU forever
    while(1) { asm volatile("hlt"); }
}