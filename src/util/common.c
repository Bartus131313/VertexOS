#include "common.h"

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
        kprint("\nCommands: help, datetime, meminfo, cpu, clear");
    } else if (strcmp(input, "datetime") == 0) {
        int h, m, s, d, mo, y;
        read_rtc_full(&h, &m, &s, &d, &mo, &y);
        
        // Notice how much cleaner this is! We use a ternary operator (?:) to add the "0" padding
        kprintf("\nDate: %s%d/%s%d/20%d  Time: %s%d:%s%d:%s%d",
            (d < 10 ? "0" : ""), d, 
            (mo < 10 ? "0" : ""), mo, y,
            (h < 10 ? "0" : ""), h, 
            (m < 10 ? "0" : ""), m, 
            (s < 10 ? "0" : ""), s
        );
    } else if (strcmp(input, "meminfo") == 0) {
        if (global_mbi->flags & MBI_FLAG_MEM) {
            // Upper memory starts at 1MB, so we add 1024KB to get the real total
            uint32_t total_kb = global_mbi->mem_lower + global_mbi->mem_upper + 1024;

            // kprintf("\nMemory Map Detected:\nLower: %d KB\nUpper: %d KB\nTotal: %d MB\n", 
            //         global_mbi->mem_lower, global_mbi->mem_upper, total_kb / 1024);

            kprintf("\nMemory Map Detected:\nTotal : %d MB\n", total_kb / 1024);

            kprintf("Blocks used: %d/%d\n", get_used_blocks(), get_total_blocks());
        } else {
            kprint("\nError: Multiboot memory info not provided.");
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