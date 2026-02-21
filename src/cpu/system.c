#include "system.h"

void print_cpu_vendor() {
    uint32_t unused, ebx, ecx, edx;
    // CPUID with EAX=0 returns the vendor string in EBX, EDX, ECX
    asm volatile("cpuid" : "=a"(unused), "=b"(ebx), "=c"(ecx), "=d"(edx) : "a"(0));

    char vendor[13];
    *((uint32_t*)vendor) = ebx;
    *((uint32_t*)(vendor + 4)) = edx;
    *((uint32_t*)(vendor + 8)) = ecx;
    vendor[12] = '\0';

    kprint("CPU Vendor: ");
    kprint(vendor);
    kprint("\n");
}