#include "util/heap.h"
#include "util/memory.h"

typedef struct heap_node {
    uint32_t size;
    uint8_t  is_free;
    struct heap_node* next;
} heap_node_t;

heap_node_t* heap_start = NULL;

void kheap_init() {
    // Ask PMM for a big chunk of memory (e.g., 1MB for the heap)
    // We'll ask for 256 blocks
    void* initial_mem = pmm_alloc_block(); 
    for(int i = 0; i < 255; i++) pmm_alloc_block(); // Grab 1MB total

    heap_start = (heap_node_t*)initial_mem;
    heap_start->size = (256 * 4096) - sizeof(heap_node_t);
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
    while (current) {
        total += current->size + sizeof(heap_node_t);
        current = current->next;
    }
    return total;
}

void* kmalloc(size_t size) {
    heap_node_t* current = heap_start;

    while (current) {
        if (current->is_free && current->size >= size) {
            // Found a spot. Split the node if there's enough extra space
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
        current = current->next;
    }
    return NULL; // Out of heap memory!
}

void kfree(void* ptr) {
    if (!ptr) return;
    heap_node_t* node = (heap_node_t*)((uint8_t*)ptr - sizeof(heap_node_t));
    node->is_free = 1;
    
    // TODO: Merge adjacent free blocks to prevent memory fragmentation.
}