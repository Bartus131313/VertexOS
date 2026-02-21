; Multiboot constants
MBALIGN  equ  1 << 0            ; Align loaded modules on page boundaries
MEMINFO  equ  1 << 1            ; Provide memory map
FLAGS    equ  MBALIGN | MEMINFO ; This is the Multiboot 'flag' field
MAGIC    equ  0x1BADB002        ; 'Magic number' lets bootloader find the header
CHECKSUM equ -(MAGIC + FLAGS)   ; Checksum of above, to prove we are multiboot

; Multiboot section
section .multiboot
align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM

; Stack section
section .bss
align 16
stack_bottom:
    resb 16384 ; 16 KB stack
stack_top:

; Start section
section .text
global _start

_start:
    cli                     ; Disable interrupts
    mov esp, stack_top      ; Point the stack pointer to our new stack
    push ebx                ; PUSH the multiboot pointer (ebx) onto the stack

    extern kmain
    call kmain              ; Jump to our C kernel
    
; Stop the CPU from executing next instructions
.hang:
    hlt                     ; Stop the CPU
    jmp .hang               ; If it wakes up, stop it again

; Import keyboard function from C
extern keyboard_handler_main

; Handles keyboard inputs
global keyboard_handler
keyboard_handler:
    pushad          ; Push EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI
    cld             ; Clear direction flag (C code expects this)
    call keyboard_handler_main
    popad           ; Restore all registers
    iretd           ; Interrupt return - THIS MUST BE IRETD

; Loads IDT
global idt_load
idt_load:
    mov eax, [esp + 4]
    lidt [eax]
    ret

; Function that handles timer
global timer_handler
timer_handler:
    pushad
    ; We don't even need a C function yet, 
    ; just tell the PIC we received the interrupt
    mov al, 0x20
    out 0x20, al
    popad
    iretd

; Flush GDT
global gdt_flush
gdt_flush:
    mov eax, [esp + 4]  ; Get the pointer to the GDT pointer
    lgdt [eax]          ; Load the GDT
    mov ax, 0x10        ; 0x10 is the offset to our Data Segment
    mov ds, ax          ; Reload all data segment registers
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp 0x08:.flush     ; 0x08 is the offset to our Code Segment. Far jump!
.flush:
    ret