/**
 *  Keyboard driver
 * 
 *  Interface for the PS/2 Keyboard driver.
 */

#ifndef VERTEX_DRIVERS_KEYBOARD_H
#define VERTEX_DRIVERS_KEYBOARD_H

#include <stdint.h>
#include <stdbool.h>

/**
 * keyboard_init
 * Initialises the PS/2 controller and enables the keyboard interrupt (IRQ1).
 */
void keyboard_init(void);

/**
 * keyboard_handler_main
 * The entry point called by the Interrupt Service Routine (ISR).
 * This reads the scancode from port 0x60 and processes it.
 */
void keyboard_handler_main(void);

/**
 * keyboard_get_last_char
 * Optional: Returns the last character typed if you prefer polling 
 * over the shell_input callback system.
 */
char keyboard_get_last_char(void);

#endif // VERTEX_DRIVERS_KEYBOARD_H