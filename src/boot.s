; MULTIBOOT HEADER
MBALIGN     equ  1<<0                   ; align loaded modules on page boundaries
MEMINFO     equ  1<<1                   ; provide memory map
FLAGS       equ  MBALIGN | MEMINFO      ; this is the Multiboot 'flag' field
MAGIC       equ  0x1BADB002             ; 'magic number' lets bootloader find the header
CHECKSUM    equ -(MAGIC + FLAGS)        ; checksum of above, to prove we are multiboot

section .multiboot
align 4
	dd MAGIC
	dd FLAGS
	dd CHECKSUM

; BOOTSTRAP STACK
section .stack
align 4
stack_bottom:
times 16384 db 0
stack_top:
 
; KERNEL START
section .text
global _start
_start:
	; GOODBYE GRUB, WELCOME K-OS -- FIRST EXECUTED INSTRUCTION
	
	; setup bootstrap stack
	mov esp, stack_top
	mov ebp, 0
 
	; push multiboot registers
    push ebx
    push eax
 
	; run global constructors
	extern _init
	call _init
 
	; start init-kernel
	extern kinit
	call kinit
	
	; run global destructors
	extern _fini
	call _fini
 
	; THIS SHOULD NEVER BE REACHED
	cli
.hang:
	hlt
	jmp .hang
