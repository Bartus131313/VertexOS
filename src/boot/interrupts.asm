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