#include "util/string.h"

int strlen(const char* str) {
    int len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

char* strcat(char* dest, const char* src) {
    // Find the end of the destination string
    int dest_len = strlen(dest);
    int i = 0;

    // Copy the source string starting at the null terminator of dest
    while (src[i] != '\0') {
        dest[dest_len + i] = src[i];
        i++;
    }

    // Add the new null terminator at the very end
    dest[dest_len + i] = '\0';

    return dest;
}

int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++; s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

int strncmp(const char* s1, const char* s2, size_t n) {
    while (n > 0 && *s1 && (*s1 == *s2)) {
        s1++;
        s2++;
        n--;
    }
    if (n == 0) return 0;
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}