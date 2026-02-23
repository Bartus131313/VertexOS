/**
 * Interrupt Descriptor Table (IDT)
 * 
 * Tells the CPU exactly what code to run when a specific event (an Interrupt) happens. 
 * Without an IDT, if you press a key or a "Divide by Zero" error occurs, 
 * the CPU doesn't know what to do and will Triple Fault (instantly restart the computer).
 */

#include <stdint.h>
#include "drivers/io.h"

struct idt_entry {
    uint16_t base_low;
    uint16_t sel;        // Code segment (0x08)
    uint8_t  always0;    // Always 0
    uint8_t  flags;      // Access flags
    uint16_t base_high;
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

struct idt_entry idt[256];
struct idt_ptr idtp;

extern void idt_load(uint32_t);

void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_low = (base & 0xFFFF);
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].sel = sel;
    idt[num].always0 = 0;
    idt[num].flags = flags;
}

void pic_remap() {
    // ICW1 - Initialize
    outb(0x20, 0x11);
    outb(0xA0, 0x11);

    // ICW2 - Set offsets (Start hardware interrupts at 32)
    outb(0x21, 0x20);
    outb(0xA1, 0x28);

    // ICW3 - Tell Master/Slave how they are wired
    outb(0x23, 0x04);
    outb(0xA3, 0x02);

    // ICW4 - Environment info
    outb(0x25, 0x01);
    outb(0xA5, 0x01);

    // Mask all interrupts except the keyboard (IRQ1)
    outb(0x21, 0xFD);
    outb(0xA1, 0xFF);

    // To enable the mouse, clear bit 4 of the Slave PIC mask 
    // (IRQ 8-15, so 12 is bit 4)
    uint8_t mask = inb(0xA1);
    outb(0xA1, mask & ~(1 << 4));
}

extern void timer_handler();
extern void keyboard_handler();
extern void mouse_handler();

void init_idt() {
    idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
    idtp.base = (uint32_t)&idt;

    for(int i = 0; i < 256; i++) {
        idt_set_gate(i, 0, 0, 0);
    }

    // Remap the PIC first!
    pic_remap();

    // 0x20 is the Timer (IRQ 0)
    idt_set_gate(0x20, (uint32_t)timer_handler, 0x08, 0x8E);
    
    // 0x21 is the Keyboard (IRQ 1)
    idt_set_gate(0x21, (uint32_t)keyboard_handler, 0x08, 0x8E);

    // 0x2C is Mouse (IRQ 12) -> 0x20 + 12 = 32 + 12 = 44 (0x2C)
    idt_set_gate(0x2C, (uint32_t)mouse_handler, 0x08, 0x8E);

    idt_load((uint32_t)&idtp);
    asm volatile("sti");
}