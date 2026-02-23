#include "drivers/input/keyboard.h"
#include "hal/io.h"
#include "kernel/shell.h"

#define KEYBOARD_DATA_PORT 0x60
#define PIC_COMMAND_PORT   0x20
#define PIC_EOI            0x20

// State tracking
static bool shift_active = false;
static bool caps_active  = false;

// Scancode to ASCII Mapping (Set 1)
static const unsigned char scancode_to_ascii[] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' '
};

static const unsigned char scancode_to_ascii_shift[] = {
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"', '~', 0,
    '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' '
};

/**
 * Handle incoming IRQ1 (Keyboard) interrupts
 */
void keyboard_handler_main(void) {
    uint8_t scancode = inb(KEYBOARD_DATA_PORT);

    // 0x80 bit (0xAA, 0xB6, etc) means a key was released (Break Code)
    if (scancode & 0x80) {
        uint8_t released_code = scancode & 0x7F;
        if (released_code == 0x2A || released_code == 0x36) {
            shift_active = false;
        }
    } 
    // Key was pressed (Make Code)
    else {
        // Check for modifier keys
        if (scancode == 0x2A || scancode == 0x36) {
            shift_active = true;
        } else if (scancode == 0x3A) {
            caps_active = !caps_active; // Toggle Caps Lock
        } else {
            // Translate scancode to character
            if (scancode < sizeof(scancode_to_ascii)) {
                char c = shift_active ? scancode_to_ascii_shift[scancode] 
                                      : scancode_to_ascii[scancode];
                
                // If character is valid, send to shell
                if (c != 0) {
                    shell_input(c);
                }
            }
        }
    }

    // Acknowledge the interrupt to the PIC
    outb(PIC_COMMAND_PORT, PIC_EOI);
}