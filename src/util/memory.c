#include "util/memory.h"

// The bitmap will live at a fixed location in memory (e.g., 2MB mark)
// In a 128MB system, this map only takes 4KB of space.
uint32_t* memory_bitmap = (uint32_t*)0x200000;
uint32_t  total_blocks  = 0;
uint32_t  used_blocks   = 0;

// Internal helpers to flip bits
static void set_bit(uint32_t bit) { memory_bitmap[bit / 32] |= (1 << (bit % 32)); }
static void clear_bit(uint32_t bit) { memory_bitmap[bit / 32] &= ~(1 << (bit % 32)); }
static int  test_bit(uint32_t bit) { return memory_bitmap[bit / 32] & (1 << (bit % 32)); }

// --- Getters ---
uint32_t pmm_get_total_blocks() {
    return total_blocks;
}

uint32_t pmm_get_used_blocks() {
    return used_blocks;
}

// --- Standard Utilities ---
void* memset(void* dest, int val, size_t count) {
    unsigned char* temp = (unsigned char*)dest;
    for (; count != 0; count--) *temp++ = (unsigned char)val;
    return dest;
}

void* memcpy(void* dest, const void* src, size_t count) {
    const char* sp = (const char*)src;
    char* dp = (char*)dest;
    for (; count != 0; count--) *dp++ = *sp++;
    return dest;
}

// --- PMM Logic ---
void pmm_init(multiboot_info_t* mbi) {
    // Calculate total memory
    uint32_t mem_kb = mbi->mem_lower + mbi->mem_upper;
    total_blocks = mem_kb / 4; 
    used_blocks = 0; // Start at 0 and count up

    // Initialize bitmap as "All Used" (1s)
    // This is safer than starting at 0.
    memset(memory_bitmap, 0xFF, (total_blocks / BLOCKS_PER_BYTE));
    
    // Mark all memory as "Free" first (0s)
    // We start from block 0 to total_blocks
    for (uint32_t i = 0; i < total_blocks; i++) {
        clear_bit(i);
    }

    // RESERVE THE PROTECTED ZONES
    // We must protect the first 1MB (VGA, BIOS, Kernel code)
    // 1MB / 4KB = 256 blocks.
    for (uint32_t i = 0; i < 256; i++) {
        set_bit(i);
        used_blocks++;
    }

    // RESERVE THE BITMAP ITSELF
    // If the bitmap is at 0x200000 (2MB mark), we need to protect those blocks too.
    // The bitmap for 128MB is roughly 1 block (4KB) large.
    uint32_t bitmap_start_block = 0x200000 / PAGE_SIZE;
    uint32_t bitmap_size_blocks = (total_blocks / 32) / PAGE_SIZE;
    if (bitmap_size_blocks == 0) bitmap_size_blocks = 1; // Minimum 1 block

    for (uint32_t i = 0; i < bitmap_size_blocks; i++) {
        set_bit(bitmap_start_block + i);
        used_blocks++;
    }
}

void* pmm_alloc_block() {
    for (uint32_t i = 0; i < total_blocks; i++) {
        if (!test_bit(i)) {
            set_bit(i);
            used_blocks++;
            return (void*)(i * PAGE_SIZE);
        }
    }
    return NULL; // Out of memory!
}

void pmm_free_block(void* ptr) {
    uint32_t address = (uint32_t)ptr;
    uint32_t block = address / PAGE_SIZE;
    clear_bit(block);
    used_blocks--;
}