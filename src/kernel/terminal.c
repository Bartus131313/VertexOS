#include "terminal.h"

int term_column = 0;
int term_row = 0;
uint8_t term_color = 0x0F;
int line_lengths[VGA_HEIGHT] = {0};

void update_cursor() {
    uint16_t pos = term_row * VGA_WIDTH + term_column;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void terminal_scroll() {
    uint16_t* video_mem = (uint16_t*)VIDEO_MEM;
    
    // 1. Move lines 1 through 23 up by one
    for (int y = 0; y < VIEW_HEIGHT - 1; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            video_mem[y * VGA_WIDTH + x] = video_mem[(y + 1) * VGA_WIDTH + x];
        }
        line_lengths[y] = line_lengths[y+1];
    }

    // 2. Clear ONLY the last line of the VIEW area (Row 23)
    // This keeps Row 24 (Status Bar) untouched!
    for (int x = 0; x < VGA_WIDTH; x++) {
        video_mem[(VIEW_HEIGHT - 1) * VGA_WIDTH + x] = (uint16_t)' ' | (uint16_t)term_color << 8;
    }
    line_lengths[VIEW_HEIGHT - 1] = 0;
}

void terminal_clear() {
    uint16_t* video_mem = (uint16_t*)VIDEO_MEM;

    // 1. Only fill the VIEW_HEIGHT (0 to 23) with spaces
    // This leaves the Status Bar (row 24) completely untouched
    for (int y = 0; y < VIEW_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            video_mem[y * VGA_WIDTH + x] = (uint16_t)' ' | (uint16_t)term_color << 8;
        }
    }

    // 2. Reset logic variables
    term_row = 0;
    term_column = 0;

    // 3. Reset line_lengths only for the viewable area
    for (int i = 0; i < VIEW_HEIGHT; i++) {
        line_lengths[i] = 0;
    }

    // 4. Move hardware cursor to top-left (0,0)
    update_cursor();
}

void terminal_putchar(char c) {
    uint16_t* video_mem = (uint16_t*)VIDEO_MEM;

    if (c == '\n') {
        line_lengths[term_row] = term_column; 
        term_column = 0;
        term_row++;
    } 
    else if (c == '\b') {
        if (term_column > 0) {
            term_column--;
        } else if (term_row > 0) {
            term_row--;
            term_column = line_lengths[term_row];
        }
        video_mem[term_row * VGA_WIDTH + term_column] = (uint16_t)' ' | (uint16_t)term_color << 8;
    } 
    else {
        video_mem[term_row * VGA_WIDTH + term_column] = (uint16_t)c | (uint16_t)term_color << 8;
        term_column++;
        line_lengths[term_row] = term_column;
    }

    // --- LOGIC CHANGE START ---

    // 1. Handle Horizontal Wrap
    if (term_column >= VGA_WIDTH) {
        line_lengths[term_row] = VGA_WIDTH;
        term_column = 0;
        term_row++;
    }

    // 2. Handle Vertical Scrolling
    // If the next line to type on is the STATUS_ROW, we scroll instead.
    if (term_row >= VIEW_HEIGHT) {
        terminal_scroll();
        term_row = VIEW_HEIGHT - 1; // Stay on the last line of the view
    }

    // --- LOGIC CHANGE END ---

    update_cursor();
}