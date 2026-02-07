# studyForth

A tiny Forth for 8051/8052 (mcs51/52), z80, and 8085 (mcs-85) development boards.

## Design

### Execution Model

* Threading?

### Primitives

List of 24 Primitives for a tiny Forth

#### Core Execution: the heart of the inner interpreter

1. EXIT — return from a word 
2. EXECUTE — execute an xt 
3. LIT — push literal from instruction stream 
4. BRANCH — unconditional jump 
5. 0BRANCH — conditional jump  

#### Data Stack

6. DROP 
7. DUP 
8. SWAP 
9. OVER  

#### Return Stack: for control structures and loops

10. &gtR — move from data → return stack 
11. R> — move from return → data stack 
12. R@ — copy top of return stack  

#### Memory Access: essential for variables, CREATE/DOES>, buffers, dictionary building

13. @ — fetch cell 
14. ! — store cell 
15. C@ — fetch byte 
16. C! — store byte 

(If your Forth is 8‑bit, @/! may be synonyms for C@/C!.)

#### Arithmetic & Logic: enough to build the rest in high‑level Forth

17. + 
18. AND 
19. OR 
20. XOR 

(XOR is extremely useful on the 8051 and cheap to implement)

#### I/O: minimal but sufficient for an interactive system

21. KEY — read a character 
22. EMIT — output a character 

(These will be hardware‑specific)

#### Dictionary & Compiler Helpers: These make : and ; trivial to implement.

23. HERE — address of next free dictionary byte 
24. ALLOT — adjust dictionary pointer 

(You can implement , in high‑level Forth using HERE, !, and ALLOT.)

With these 24 primitives, we can write the entire rest of the system in high‑level Forth:

- : and ; 
- IMMEDIATE 
- CREATE / DOES> 
- VARIABLE, CONSTANT 
- IF, ELSE, THEN 
- BEGIN, UNTIL, AGAIN, WHILE, REPEAT 
- DO, LOOP, +LOOP 
- , and C, 
- numeric output 
- interpreter loop 

### To Do List

This is enough to bootstrap a full Forth environment from scratch.

Next Steps:

- map these primitives to 8051 registers 
- write each one in AS31 assembly 
- design the dictionary header 
- build the cold‑start sequence 
- or sketch the high‑level bootstrap code for : and ;  
