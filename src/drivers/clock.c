#include "drivers/clock.h"
#include "hal/io.h"

#define CMOS_ADDR 0x70
#define CMOS_DATA 0x71

// RTC Registers
enum {
    RTC_SEC = 0x00, RTC_MIN = 0x02, RTC_HOUR = 0x04,
    RTC_DAY = 0x07, RTC_MONTH = 0x08, RTC_YEAR = 0x09,
    RTC_STAT_A = 0x0A, RTC_STAT_B = 0x0B
};

static int get_update_in_progress_flag() {
    outb(CMOS_ADDR, RTC_STAT_A);
    return (inb(CMOS_DATA) & 0x80);
}

static uint8_t get_rtc_register(int reg) {
    outb(CMOS_ADDR, reg);
    return inb(CMOS_DATA);
}

void rtc_get_time(rtc_time_t* time) {
    rtc_time_t last;

    // Ensure we didn't read during a tick update
    do {
        last = *time;
        while (get_update_in_progress_flag()); 

        time->second = get_rtc_register(RTC_SEC);
        time->minute = get_rtc_register(RTC_MIN);
        time->hour   = get_rtc_register(RTC_HOUR);
        time->day    = get_rtc_register(RTC_DAY);
        time->month  = get_rtc_register(RTC_MONTH);
        time->year   = get_rtc_register(RTC_YEAR);
    } while ( (last.second != time->second) || (last.minute != time->minute) || (last.hour != time->hour) );

    uint8_t registerB = get_rtc_register(RTC_STAT_B);

    // Convert BCD to Binary if necessary (Bit 2 of Register B)
    if (!(registerB & 0x04)) {
        time->second = (time->second & 0x0F) + ((time->second / 16) * 10);
        time->minute = (time->minute & 0x0F) + ((time->minute / 16) * 10);
        time->hour   = ( (time->hour & 0x0F) + (((time->hour & 0x70) / 16) * 10) ) | (time->hour & 0x80);
        time->day    = (time->day & 0x0F) + ((time->day / 16) * 10);
        time->month  = (time->month & 0x0F) + ((time->month / 16) * 10);
        time->year   = (time->year & 0x0F) + ((time->year / 16) * 10);
    }

    // Convert 12 hour clock to 24 hour clock if necessary (Bit 1 of Register B)
    if (!(registerB & 0x02) && (time->hour & 0x80)) {
        time->hour = ((time->hour & 0x7F) + 12) % 24;
    }

    // Year handling (Assume we are in the 2000s)
    time->year += 2000; 
}