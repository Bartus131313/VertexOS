#ifndef MULTIBOOT_H
#define MULTIBOOT_H

#include <stdint.h>

#define MULTIBOOT_INFO_MEMORY 0x00000001

// The definition of the structure
typedef struct {
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    // ... add more fields here if needed later
} multiboot_info_t;

// 'extern' tells other files: "The variable 'mbi' exists in another .c file"
extern multiboot_info_t* global_mbi;

#endif