#ifndef MOUSE_H
#define MOUSE_H

#include <stdint.h>
#include <stdbool.h>

// Global mouse state accessible by the rest of the OS
extern int32_t mouse_x;
extern int32_t mouse_y;
extern bool mouse_left_click;
extern bool mouse_right_click;
extern bool mouse_middle_click;

void mouse_init();
void mouse_handler_main(); // To be called from your IRQ 12 assembly handler

#endif