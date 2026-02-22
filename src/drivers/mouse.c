#include "mouse.h"
#include "drivers/io.h"
#include "graphics/vesa.h"

int32_t mouse_x = 0;
int32_t mouse_y = 0;
bool mouse_left_click = false;
bool mouse_right_click = false;
bool mouse_middle_click = false;

uint8_t mouse_cycle = 0;
int8_t mouse_byte[3];

// Wait until the PS/2 controller is ready
void mouse_wait(uint8_t a_type) {
    uint32_t timeout = 100000;
    if (a_type == 0) {
        // Wait to read
        while (timeout--) { if ((inb(0x64) & 1) == 1) return; }
    } else {
        // Wait to write
        while (timeout--) { if ((inb(0x64) & 2) == 0) return; }
    }
}

// Send a command to the mouse
void mouse_write(uint8_t write) {
    mouse_wait(1);
    outb(0x64, 0xD4); // Tell controller we are sending to the mouse
    mouse_wait(1);
    outb(0x60, write); // Send the actual command
}

uint8_t mouse_read() {
    mouse_wait(0);
    return inb(0x60);
}

void mouse_init() {
    uint8_t status;

    // Enable auxiliary device (the mouse)
    mouse_wait(1);
    outb(0x64, 0xA8);

    // Enable IRQ 12
    mouse_wait(1);
    outb(0x64, 0x20); // Get status command
    mouse_wait(0);
    status = (inb(0x60) | 2); // Set bit 1 to enable IRQ 12
    mouse_wait(1);
    outb(0x64, 0x60); // Set status command
    mouse_wait(1);
    outb(0x60, status);

    while (inb(0x64) & 0x01) {
        inb(0x60);
    }

    // Tell mouse to use default settings
    mouse_write(0xF6);
    mouse_read(); // Acknowledge

    // Enable data reporting (starts sending interrupts)
    mouse_write(0xF4);
    mouse_read(); // Acknowledge
}

// This is called every time the mouse moves or clicks
void mouse_handler_main() {
    uint8_t status = inb(0x64);
    if (!(status & 0x01)) return; // No data to read

    uint8_t data = inb(0x60);

    // If we are looking for the FIRST byte, check bit 3
    if (mouse_cycle == 0 && !(data & 0x08)) {
        return; // This isn't the start of a packet! Skip it.
    }

    mouse_byte[mouse_cycle++] = data;

    if (mouse_cycle == 3) {
        mouse_cycle = 0;
        
        // Byte 0: [Y_ovfl, X_ovfl, Y_sign, X_sign, 1, Middle, Right, Left]
        mouse_left_click   = (mouse_byte[0] & 0x01);
        mouse_right_click  = (mouse_byte[0] & 0x02);
        mouse_middle_click = (mouse_byte[0] & 0x04);

        // Movement with Sign Extension
        // If the sign bit is set, we need to make it a negative 32-bit integer
        int32_t rel_x = (int32_t)mouse_byte[1];
        int32_t rel_y = (int32_t)mouse_byte[2];

        if (mouse_byte[0] & 0x10) rel_x |= 0xFFFFFF00; // X Sign Bit
        if (mouse_byte[0] & 0x20) rel_y |= 0xFFFFFF00; // Y Sign Bit

        mouse_x += rel_x;
        mouse_y -= rel_y; // Y is inverted on screen

        // Clamp to screen
        if (mouse_x < 0) mouse_x = 0;
        if (mouse_y < 0) mouse_y = 0;
        if (mouse_x >= vesa_screen_width) mouse_x = vesa_screen_width - 1;
        if (mouse_y >= vesa_screen_height) mouse_y = vesa_screen_height - 1;
    }
}