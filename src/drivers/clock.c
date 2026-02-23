#include "clock.h"

int get_update_in_progress_flag() {
    outb(CMOS_ADDR, 0x0A);
    return (inb(CMOS_DATA) & 0x80);
}

uint8_t get_rtc_register(int reg) {
    outb(CMOS_ADDR, reg);
    return inb(CMOS_DATA);
}

void read_rtc(int* h, int* m, int* s) {
    while (get_update_in_progress_flag()); // Wait for update
    
    *s = get_rtc_register(0x00);
    *m = get_rtc_register(0x02);
    *h = get_rtc_register(0x04);

    // Convert BCD to binary
    *s = (*s & 0x0F) + ((*s / 16) * 10);
    *m = (*m & 0x0F) + ((*m / 16) * 10);
    *h = (*h & 0x0F) + ((*h / 16) * 10);
}

void read_rtc_full(int* h, int* m, int* s, int* day, int* month, int* year) {
    while (get_update_in_progress_flag());
    *s = get_rtc_register(0x00);
    *m = get_rtc_register(0x02);
    *h = get_rtc_register(0x04);
    *day = get_rtc_register(0x07);
    *month = get_rtc_register(0x08);
    *year = get_rtc_register(0x09);

    // BCD conversion for all
    *s = (*s & 0x0F) + ((*s / 16) * 10);
    *m = (*m & 0x0F) + ((*m / 16) * 10);
    *h = (*h & 0x0F) + ((*h / 16) * 10);
    *day = (*day & 0x0F) + ((*day / 16) * 10);
    *month = (*month & 0x0F) + ((*month / 16) * 10);
    *year = (*year & 0x0F) + ((*year / 16) * 10);
}