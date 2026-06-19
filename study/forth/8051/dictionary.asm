; dictionary.asm — Word headers and initial dictionary
; 
; This file defines:
; 
;     Name fields
; 
;     Link fields
; 
;     Code fields
; 
;     Initial built-in words
; 
; You can generate dictionary entries with macros to keep things readable.

; +------------------+
; | Link Field (2B)  |  pointer to previous word
; +------------------+
; | Name Length/Flags|
; +------------------+
; | Name Characters  |
; +------------------+
; | CFA (2B)         |  pointer to codeword (or primitive index)
; +------------------+
; | Parameter Field  |  (varies)
; +------------------+


