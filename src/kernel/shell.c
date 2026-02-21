#include "shell.h"

char command_buffer[MAX_COMMAND_LEN + 1];
int buffer_idx = 0;

void shell_print_prefix() {
    kprint("\nSystem >:")
}

void shell_input(char c) {
    // 1. Handle Enter Key
    if (c == '\n') {
        command_buffer[buffer_idx] = '\0'; // Null-terminate the string
        
        if (buffer_idx > 0) {
            execute_command(command_buffer);
        }
        
        // Reset for the next command
        buffer_idx = 0;
        shell_print_prefix();
    } 
    // 2. Handle Backspace
    else if (c == '\b') {
        if (buffer_idx > 0) {
            buffer_idx--;
            terminal_putchar('\b'); // Erase from screen
        }
    } 
    // 3. Handle Regular Characters
    else {
        if (buffer_idx < MAX_COMMAND_LEN) {
            command_buffer[buffer_idx++] = c;
            terminal_putchar(c); // Show on screen
        }
    }
}