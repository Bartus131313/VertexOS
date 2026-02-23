/**
 * Input/Output Bus
 * 
 * Port-mapped I/O (PMIO) wrappers for x86 communication.
 * Uses 'statis inline' to eliminate function call overhead.
 */

#ifndef VERTEX_HAL_IO_H
#define VERTEX_HAL_IO_H

#include <stdint.h>

/**
 * Send 8-bit byte to I/O port
 */
static inline void outb(uint16_t port, uint8_t val) {
    asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

/**
 * Receive 8-bit byte from I/O port
 */
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

/**
 * Send 16-bit word to I/O port
 */
static inline void outw(uint16_t port, uint16_t val) {
    asm volatile ("outw %0, %1" : : "a"(val), "Nd"(port));
}

/**
 * Receive 16-bit word from I/O port
 */
static inline uint16_t inw(uint16_t port) {
    uint16_t ret;
    asm volatile ("inw %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

/**
 * Forces the CPU to wait for an I/O operation to complete.
 * Used for older hardware that is slower than the CPU (like the PS/2 controller).
 */
static inline void io_wait(void) {
    // Port 0x80 is unused after POST, writing to it takes ~1 microsecond
    outb(0x80, 0);
}

#endif // VERTEX_HAL_IO_H