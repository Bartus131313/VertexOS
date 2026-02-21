#ifndef COMMON_H
#define COMMON_H

#include "kernel/terminal.h"

void kprint(const char* str);

// Very useful for debugging GDT/IDT addresses!
void kprint_hex(uint32_t n);
void kprint_int(int n);

int strcmp(const char* s1, const char* s2);

void execute_command(char* input);

#endif