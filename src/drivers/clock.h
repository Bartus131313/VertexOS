/**
 *  Real-Time Clock
 * 
 *  Driver for the Motorola 6818 RTC/CMOS.
 */

#ifndef VERTEX_DRIVERS_CLOCK_H
#define VERTEX_DRIVERS_CLOCK_H

#include <stdint.h>

typedef struct {
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
    uint8_t month;
    uint32_t year;
} rtc_time_t;

/**
 * Populates the rtc_time_t struct with current hardware time.
 * Handles BCD-to-Binary conversion and 12/24 hour modes.
 */
void rtc_get_time(rtc_time_t* time);

#endif