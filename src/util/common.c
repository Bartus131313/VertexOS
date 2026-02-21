#include "common.h"

void* test_ptr = NULL;

void kprintf(const char* format, ...) {
    va_list args;
    va_start(args, format);

    for (int i = 0; format[i] != '\0'; i++) {
        if (format[i] == '%') {
            i++; // Skip the '%' and look at the next character
            switch (format[i]) {
                case 'd': {
                    int val = va_arg(args, int);
                    kprint_int(val);
                    break;
                }
                case 's': {
                    char* val = va_arg(args, char*);
                    kprint(val);
                    break;
                }
                case 'x': {
                    uint32_t val = va_arg(args, uint32_t);
                    kprint_hex(val);
                    break;
                }
                case 'c': {
                    char val = (char)va_arg(args, int); // char is promoted to int in va_arg
                    terminal_putchar(val);
                    break;
                }
                case '%': {
                    terminal_putchar('%');
                    break;
                }
                default: {
                    // If it's an unknown format, just print it as-is
                    terminal_putchar('%');
                    terminal_putchar(format[i]);
                    break;
                }
            }
        } else {
            // Normal character
            terminal_putchar(format[i]);
        }
    }

    va_end(args);
}

void kprint(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        terminal_putchar(str[i]);
    }
}

void kprint_hex(uint32_t n) {
    char* hex_chars = "0123456789ABCDEF";
    kprint("0x");
    for (int i = 28; i >= 0; i -= 4) {
        terminal_putchar(hex_chars[(n >> i) & 0xF]);
    }
}

void kprint_int(int n) {
    if (n == 0) {
        terminal_putchar('0');
        return;
    }

    if (n < 0) {
        terminal_putchar('-');
        n = -n;
    }

    char buffer[12]; // Big enough for a 32-bit int
    int i = 0;
    while (n > 0) {
        buffer[i++] = (n % 10) + '0';
        n /= 10;
    }

    // The digits are in reverse order, so print them backwards
    while (i > 0) {
        terminal_putchar(buffer[--i]);
    }
}

void execute_command(char* input) {
    if (strcmp(input, "help") == 0) {
        kprint("\nCommands: help, datetime, meminfo, cpu, testalloc, testfree, clear");
    } else if (strcmp(input, "datetime") == 0) {
        int h, m, s, d, mo, y;
        read_rtc_full(&h, &m, &s, &d, &mo, &y);
        
        // Notice how much cleaner this is! We use a ternary operator (?:) to add the "0" padding
        kprintf("\nDate: %s%d/%s%d/20%d  Time: %s%d:%s%d:%s%d  UTC+0",
            (d < 10 ? "0" : ""), d, 
            (mo < 10 ? "0" : ""), mo, y,
            (h < 10 ? "0" : ""), h, 
            (m < 10 ? "0" : ""), m, 
            (s < 10 ? "0" : ""), s
        );
    } else if (strcmp(input, "meminfo") == 0) {
        uint32_t p_used = pmm_get_used_blocks();
        uint32_t p_total = pmm_get_total_blocks();
        size_t h_used = kheap_get_used_bytes();
        size_t h_total = kheap_get_total_bytes();

        kprintf("\n--- %s Memory Report ---", SYSTEM_NAME);
        
        // Physical Layer (PMM)
        kprintf("\nPhysical RAM  : %d / %d blocks", p_used, p_total);
        kprintf("\nPhysical Usage: %d%%", (p_used * 100) / p_total);
        
        kprintf("\n------------------------------");
        
        // Heap Layer (kmalloc)
        // Converting bytes to KB for readability
        kprintf("\nKernel Heap   : %d / %d KB", h_used / 1024, h_total / 1024);
        
        // Bonus: Show how many "free" blocks are left in the physical pool
        kprintf("\nAvailable RAM : %d MB", ((p_total - p_used) * 4) / 1024);
        kprintf("\n");
    } else if (strcmp(input, "testalloc") == 0) {
        if (test_ptr != NULL) {
            kprint("\nAlready allocated! Use 'testfree' first.");
        } else {
            kprint("\nAllocating 10KB...");
            test_ptr = kmalloc(1024 * 10);
            if (test_ptr) {
                kprintf("\nSuccess! Pointer: %x", (uint32_t)test_ptr);
            } else {
                kprint("\nFailed to allocate.");
            }
        }
    } 
    else if (strcmp(input, "testfree") == 0) {
        if (test_ptr) {
            kfree(test_ptr);
            test_ptr = NULL;
            kprint("\nMemory freed and coalesced.");
        } else {
            kprint("\nNothing to free.");
        }
    } else if (strcmp(input, "cpu") == 0) {
        kprint("\n");
        print_cpu_vendor();
    } else if (strcmp(input, "clear") == 0) {
        terminal_clear(); // Your clear screen function
    } else {
        kprint("\nUnknown command.");
    }
}