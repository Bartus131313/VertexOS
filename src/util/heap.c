#include "util/heap.h"
#include "util/memory.h"

typedef struct heap_node {
    uint32_t size;
    uint8_t  is_free;
    struct heap_node* next;
} heap_node_t;

heap_node_t* heap_start = NULL;

// void kheap_init() {
//     // Ask PMM for a big chunk of memory (e.g., 1MB for the heap)
//     // We'll ask for 256 blocks
//     void* initial_mem = pmm_alloc_block(); 
//     for(int i = 0; i < 255; i++) pmm_alloc_block(); // Grab 1MB total

//     heap_start = (heap_node_t*)initial_mem;
//     heap_start->size = (256 * 4096) - sizeof(heap_node_t);
//     heap_start->is_free = 1;
//     heap_start->next = NULL;
// }

void kheap_init() {
    void* initial_mem = pmm_alloc_block(); 
    
    heap_start = (heap_node_t*)initial_mem;
    heap_start->size = PAGE_SIZE - sizeof(heap_node_t);
    heap_start->is_free = 1;
    heap_start->next = NULL;
}

size_t kheap_get_used_bytes() {
    size_t used = 0;
    heap_node_t* current = heap_start;
    while (current) {
        if (!current->is_free) {
            // We add the user size + the size of the header itself
            used += current->size + sizeof(heap_node_t);
        }
        current = current->next;
    }
    return used;
}

size_t kheap_get_total_bytes() {
    size_t total = 0;
    heap_node_t* current = heap_start;
    int safety_counter = 0; 
    while (current && safety_counter < 100) { // Stop after 100 nodes
        total += current->size + sizeof(heap_node_t);
        current = current->next;
        safety_counter++;
    }
    return total;
}

void* kmalloc(size_t size) {
    heap_node_t* current = heap_start;
    heap_node_t* last = NULL;

    // Try to find an existing free block
    while (current) {
        if (current->is_free && current->size >= size) {
            // Found a spot! (Split logic stays the same as before)
            if (current->size > size + sizeof(heap_node_t) + 4) {
                heap_node_t* next_node = (heap_node_t*)((uint8_t*)current + sizeof(heap_node_t) + size);
                next_node->size = current->size - size - sizeof(heap_node_t);
                next_node->is_free = 1;
                next_node->next = current->next;
                current->size = size;
                current->next = next_node;
            }
            current->is_free = 0;
            return (void*)((uint8_t*)current + sizeof(heap_node_t));
        }
        last = current;
        current = current->next;
    }

    // If we got here, we need to GROW
    // Calculate exactly how many blocks we need
    uint32_t blocks_needed = (size + sizeof(heap_node_t) + PAGE_SIZE - 1) / PAGE_SIZE;
    
    // Grab the first block
    void* new_block = pmm_alloc_block();
    if (!new_block) return NULL;

    // Clear the memory immediately so no old "next" pointers exist
    memset(new_block, 0, sizeof(heap_node_t));

    // Reserve the rest. Note: In a real kernel, we'd check if these are contiguous.
    // For now, we assume they are or treat them as one large chunk.
    for(uint32_t i = 1; i < blocks_needed; i++) {
        pmm_alloc_block(); 
    }

    heap_node_t* new_node = (heap_node_t*)new_block;
    new_node->size = (blocks_needed * PAGE_SIZE) - sizeof(heap_node_t);
    new_node->is_free = 0; 
    new_node->next = NULL;

    // Attach to the end of the list
    if (last) {
        last->next = new_node;
    } else {
        // This handles the case where heap_start was NULL
        heap_start = new_node;
    }

    return (void*)((uint8_t*)new_node + sizeof(heap_node_t));
}

void kheap_dump() {
    heap_node_t* current = heap_start;
    int index = 0;

    kprintf("\nIndex | Address    | Size       | Status");
    kprintf("\n---------------------------------------");

    while (current) {
        kprintf("\n[%d]   | %x | %d bytes | %s", 
                index, 
                (uint32_t)current, 
                current->size, 
                current->is_free ? "FREE" : "USED");
        
        current = current->next;
        index++;
    }
    kprintf("\n---------------------------------------");
}

void kfree(void* ptr) {
    if (!ptr) return;
    heap_node_t* node = (heap_node_t*)((uint8_t*)ptr - sizeof(heap_node_t));
    node->is_free = 1;

    heap_node_t* current = heap_start;
    while (current && current->next) {
        if (current->is_free && current->next->is_free) {
            // Only merge if they are physically adjacent
            uint32_t current_end = (uint32_t)current + sizeof(heap_node_t) + current->size;
            if (current_end == (uint32_t)current->next) {
                current->size += current->next->size + sizeof(heap_node_t);
                current->next = current->next->next;
                continue; // Stay on this node to see if we can merge the next one too
            }
        }
        current = current->next;
    }
}