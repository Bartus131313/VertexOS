; Multiboot macros
MBOOT_PAGE_ALIGN    equ 1<<0
MBOOT_MEM_INFO      equ 1<<1
MBOOT_VIDEO_MODE    equ 1<<2 ; <--- THIS BIT IS KEY
MBOOT_MAGIC         equ 0x1BADB002
MBOOT_FLAGS         equ MBOOT_PAGE_ALIGN | MBOOT_MEM_INFO | MBOOT_VIDEO_MODE
MBOOT_CHECKSUM      equ -(MBOOT_MAGIC + MBOOT_FLAGS)

section .multiboot
align 4
    dd MBOOT_MAGIC
    dd MBOOT_FLAGS
    dd MBOOT_CHECKSUM

    ; These are 5 "Reserved" fields for aout kludge (set to 0)
    dd 0, 0, 0, 0, 0

    ; Video Mode Preference (The VESA config)
    dd 0    ; 0 = Linear Framebuffer
    dd 1280 ; Width
    dd 720  ; Height
    dd 32   ; Depth (Bits Per Pixel)

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

; Import Keyboard function from C
extern keyboard_handler_main

; Handles Keyboard inputs
global keyboard_handler
keyboard_handler:
    pushad          ; Push EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI
    cld             ; Clear direction flag (C code expects this)
    call keyboard_handler_main
    popad           ; Restore all registers
    iretd           ; Interrupt return - THIS MUST BE IRETD

; Import Mouse handler function from C
extern mouse_handler_main

; Handles Mouse inputs
global mouse_handler
mouse_handler:
    pusha            ; Save all registers (eax, ebx, ecx, edx, etc.)
    
    call mouse_handler_main
    
    mov al, 0x20
    out 0xA0, al     ; Send EOI to Slave PIC (because IRQ 12 is on Slave)
    out 0x20, al     ; Send EOI to Master PIC
    
    popa             ; Restore all registers
    iret             ; Return from interrupt

; sse_memcpy(void* dest, void* src, uint32_t n)
global sse_memcpy
sse_memcpy:
    push ebp
    mov ebp, esp
    push edi
    push esi

    mov edi, [ebp + 8]   ; Destination
    mov esi, [ebp + 12]  ; Source
    mov ecx, [ebp + 16]  ; Number of bytes
    
    shr ecx, 4           ; Divide bytes by 16 (since we move 16 per loop)

.loop:
    movdqu xmm0, [esi]   ; Load 16 bytes from RAM into XMM0 register
    movdqu [edi], xmm0   ; Store 16 bytes from XMM0 into Video RAM
    add esi, 16
    add edi, 16
    loop .loop

    pop esi
    pop edi
    mov esp, ebp
    pop ebp
    ret

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