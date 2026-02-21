#include "common.h"
#include "kernel/multiboot.h"

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

int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++; s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

void execute_command(char* input) {
    if (strcmp(input, "help") == 0) {
        kprint("\nCommands: help, datetime, meminfo, cpu, clear");
    } else if (strcmp(input, "datetime") == 0) {
        int h, m, s, d, mo, y;
        read_rtc_full(&h, &m, &s, &d, &mo, &y);
        
        kprint("\nDate: ");
        // Format: DD/MM/20YY
        if (d < 10) kprint("0"); kprint_int(d);
        kprint("/");
        if (mo < 10) kprint("0"); kprint_int(mo);
        kprint("/20"); kprint_int(y);

        kprint("  Time: ");
        // Format: HH:MM:SS
        if (h < 10) kprint("0"); kprint_int(h);
        kprint(":");
        if (m < 10) kprint("0"); kprint_int(m);
        kprint(":");
        if (s < 10) kprint("0"); kprint_int(s);
    } else if (strcmp(input, "meminfo") == 0) {
        if (global_mbi->flags & MBI_FLAG_MEM) {
            // Upper memory starts at 1MB, so we add 1024KB to get the real total
            uint32_t total_kb = global_mbi->mem_lower + global_mbi->mem_upper + 1024;
            
            kprint("\nMemory Map Detected:");
            kprint("\nLower: "); kprint_int(global_mbi->mem_lower); kprint(" KB");
            kprint("\nUpper: "); kprint_int(global_mbi->mem_upper); kprint(" KB");
            kprint("\nTotal: "); kprint_int(total_kb / 1024); kprint(" MB");
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