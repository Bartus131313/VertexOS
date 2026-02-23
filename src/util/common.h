#ifndef COMMON_H
#define COMMON_H

#include <stdarg.h>

#include "kernel/terminal.h"
#include "kernel/multiboot.h"
#include "libc/string.h"
#include "libc/memory.h"
#include "libc/heap.h"
#include "util/constans.h"
#include "drivers/clock.h"

void kprintf(const char* format, ...);
void kprint(const char* str);

void kprint_hex(uint32_t n);
void kprint_int(int n);

void execute_command(char* input);

#endif