#ifndef CLOCK_H
#define CLOCK_H

#include "io.h"

#define CMOS_ADDR 0x70
#define CMOS_DATA 0x71

int get_update_in_progress_flag();

uint8_t get_rtc_register(int reg);

void read_rtc(int* h, int* m, int* s);
void read_rtc_full(int* h, int* m, int* s, int* day, int* month, int* year);

#endif