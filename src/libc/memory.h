#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>
#include <stddef.h>
#include "kernel/multiboot.h"

// --- Constants ---
#define PAGE_SIZE 4096      // 4KB pages
#define BLOCKS_PER_BYTE 8

// --- Getters ---
uint32_t pmm_get_total_blocks();
uint32_t pmm_get_used_blocks();

// --- Byte-Level Utilities (Standard C) ---
void* memset(void* dest, int val, size_t count);
void* memcpy(void* dest, const void* src, size_t count);
int   memcmp(const void* s1, const void* s2, size_t count);
int   strlen(const char* str);

// --- Physical Memory Manager (PMM) ---
// Initializes the bitmap based on RAM detected by Multiboot
void pmm_init(multiboot_info_t* mbi);

// Allocates one 4KB block and returns its physical address
void* pmm_alloc_block();

// Frees a 4KB block at the given address
void pmm_free_block(void* ptr);

#endif