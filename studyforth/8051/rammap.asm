; rammap.asm — Memory layout and constants
; 
; The 8051 has quirky RAM, so it helps to isolate all memory definitions.
; 
; Typical contents
; 
;     Data stack base and size
; 
;     Return stack base and size
; 
;     User variables
; 
;     Pointers (DP, IP, W, etc.)
; 
;     EQU definitions for ports, constants, flags
; 
; This file prevents magic numbers from leaking everywhere.

;
; 8051 has 128 bytes RAM 0x00-0x7F
;
; 0x00 - 0x1F are register banks to avoid
;

; Data Stack Pointer

.EQU	DSP, 0x20

; Return Stack Pointer

.EQU	RSP, 0x21

; Instruction Pointer

.EQU	IP_L, 0x22
.EQU 	IP_H, 0x23

; Temporary variables if needed

.EQU	TMP0, 0x24
.EQU	TMP1, 0x25
