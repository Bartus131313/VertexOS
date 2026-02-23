#ifndef STRING_H
#define STRING_H

#include <stddef.h>
#include <stdint.h>

// Finds the length of a string
int strlen(const char* str);

// Merges source into destination
char* strcat(char* dest, const char* src);

// Compares two strings
int strcmp(const char* s1, const char* s2);
int strncmp(const char* s1, const char* s2, size_t n);

#endif