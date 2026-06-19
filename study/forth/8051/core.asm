; core.asm — The inner interpreter (NEXT loop)
; 
; This is the heart of your Forth.
; 
; Typical contents
; 
;     NEXT: loop (fetch code pointer, jump to code field)
; 
;     Threaded-code execution model (direct-threaded or subroutine-threaded)
; 
;     Return stack operations
; 
;     Execution token handling
; 
; This file defines the execution model, so it’s the most important one.

; ------------------------------------------------------------
; PUSH  ( A = low byte, B = high byte )
; pushes a 16-bit value onto the data stack
; ------------------------------------------------------------

PUSH16:
	MOV   @DSP, A      ; store low byte
	INC   DSP
	MOV   @DSP, B      ; store high byte
	INC   DSP
	RET

; ------------------------------------------------------------
; POP  ( returns A = low byte, B = high byte )
; pops a 16-bit value from the data stack
; ------------------------------------------------------------

POP16:
    DEC   DSP
    MOV   B, @DSP      ; high byte
    DEC   DSP
    MOV   A, @DSP      ; low byte
    RET

; ------------------------------------------------------------
; RPUSH  ( pushes IP_L and IP_H onto return stack )
; ------------------------------------------------------------

RPUSH:
    MOV   A, IP_L
    MOV   @RSP, A
    INC   RSP

    MOV   A, IP_H
    MOV   @RSP, A
    INC   RSP
    RET

; ------------------------------------------------------------
; RPOP  ( pops into IP_H and IP_L )
; ------------------------------------------------------------

RPOP:
    DEC   RSP
    MOV   IP_H, @RSP

    DEC   RSP
    MOV   IP_L, @RSP
    RET

; ------------------------------------------------------------
; EXIT  ( -- )   Return from a high-level Forth word
; Pops IP from return stack and resumes NEXT
; ------------------------------------------------------------

EXIT:
    ; Pop IP_H and IP_L from return stack
    DEC   RSP
    MOV   IP_H, @RSP

    DEC   RSP
    MOV   IP_L, @RSP

    SJMP  NEXT

; ------------------------------------------------------------
; NEXT — inner interpreter for ITC Forth
; Uses IP_L/IP_H as instruction pointer
; Codewords are 1-byte indexes into jump table
; ------------------------------------------------------------

NEXT:
    ; Load DPTR = IP
    MOV   DPL, IP_L
    MOV   DPH, IP_H

    ; Fetch CFA low byte
    CLR   A
    MOVC  A, @A+DPTR
    MOV   TMP0, A
    INC   DPTR

    ; Fetch CFA high byte
    CLR   A
    MOVC  A, @A+DPTR
    MOV   TMP1, A
    INC   DPTR

    ; Store updated IP
    MOV   IP_L, DPL
    MOV   IP_H, DPH

    ; Now DPTR = CFA
    MOV   DPL, TMP0
    MOV   DPH, TMP1

    ; Fetch codeword (primitive index)
    CLR   A
    MOVC  A, @A+DPTR

    ; Jump through jump table
    MOV   DPTR, #jump_table
    JMP   @A+DPTR


