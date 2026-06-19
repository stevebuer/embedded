; primitives.asm — Assembly implementations of core words
; 
; These are the low-level words that everything else builds on.
; 
; Examples
; 
;     Stack ops: DUP, DROP, SWAP, OVER
; 
;     Arithmetic: +, -, AND, OR, XOR
; 
;     Branching: BRANCH, 0BRANCH
; 
;     Memory ops: @, !
; 
;     I/O primitives if needed
; 
; Keeping these in their own file makes it easy to optimize later.

; ------------------------------------------------------------
; DUP  ( x -- x x )
; Duplicate top 16-bit value on data stack
; ------------------------------------------------------------

prim_DUP:
    ; Pop top of stack into A (low) and B (high)
    LCALL  POP16

    ; Push it back twice
    LCALL  PUSH16
    LCALL  PUSH16

    SJMP   NEXT

; ------------------------------------------------------------
; DUP  ( x -- x x )
; Duplicate top 16-bit value on data stack
; ------------------------------------------------------------

prim_DUP:
    ; Pop top of stack into A (low) and B (high)
    LCALL  POP16

    ; Push it back twice
    LCALL  PUSH16
    LCALL  PUSH16

    SJMP   NEXT

; ------------------------------------------------------------
; SWAP  ( x1 x2 -- x2 x1 )
; ------------------------------------------------------------

prim_SWAP:
    ; Pop x2 into A/B
    LCALL  POP16
    MOV    TMP0, A
    MOV    TMP1, B

    ; Pop x1 into A/B
    LCALL  POP16

    ; Push x2
    MOV    A, TMP0
    MOV    B, TMP1
    LCALL  PUSH16

    ; Push x1
    LCALL  PUSH16

    SJMP   NEXT

; ------------------------------------------------------------
; OVER  ( x1 x2 -- x1 x2 x1 )
; ------------------------------------------------------------

prim_OVER:
    ; Pop x2
    LCALL  POP16
    MOV    TMP0, A
    MOV    TMP1, B

    ; Pop x1
    LCALL  POP16
    MOV    TMP2, A
    MOV    TMP3, B

    ; Push x1
    MOV    A, TMP2
    MOV    B, TMP3
    LCALL  PUSH16

    ; Push x2
    MOV    A, TMP0
    MOV    B, TMP1
    LCALL  PUSH16

    ; Push x1 again
    MOV    A, TMP2
    MOV    B, TMP3
    LCALL  PUSH16

    SJMP   NEXT

