; startup.asm — Reset, vectors, and hardware init
; 
; This file handles everything the 8051 must do before Forth can run.
; 
; Typical contents
;
;    Reset vector
;    Interrupt vectors (even if unused, stub them cleanly)
;    Stack pointer initialization
;    Internal RAM setup
;    Jump to the Forth cold start routine
;
; This file stays small and rarely changes.
