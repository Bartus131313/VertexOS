#ifndef TERMINAL_H
#define TERMIANL_H

#include "drivers/io.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VIDEO_MEM 0xB8000
#define VIEW_HEIGHT 24
#define STATUS_ROW 24

extern int term_column;
extern int term_row;
extern uint8_t term_color;
extern int line_lengths[VGA_HEIGHT];

void terminal_clear();

void terminal_putchar(char c);

#endif