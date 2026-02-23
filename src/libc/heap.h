#ifndef HEAP_H
#define HEAP_H

#include <stdint.h>
#include <stddef.h>

// Initializes the heap by requesting blocks from the PMM
void kheap_init();

// Dumps heap memory into the terminal
void kheap_dump();

// Get Heap used bytes
size_t kheap_get_used_bytes();
size_t kheap_get_total_bytes();

// The "Standard" allocation functions
void* kmalloc(size_t size);
void kfree(void* ptr);

#endif