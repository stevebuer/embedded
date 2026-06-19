You’re at a really exciting point in the project. You now have:

- A working stack engine (PUSH16, POP16, RPUSH, RPOP)
- Core control words (EXIT, NEXT)
- Basic primitives (DUP, DROP, SWAP, OVER)
- A solid understanding of the dictionary structure

So the natural question is: **what assembly should you write next to keep the Forth kernel growing in the right order?**

Let me give you a clear roadmap. This is the same sequence used in most successful 8‑bit Forths, because each step unlocks the next.

---

# 🧱 What to write next (in the correct order)

## **1. Arithmetic primitives**
These are simple and give you confidence that the stack engine works.

- `+`
- `-`
- `1+`
- `1-`
- `NEGATE`

These all follow the same pattern:

1. POP16 → A/B  
2. POP16 → C/D  
3. Perform 16‑bit arithmetic  
4. PUSH16 result  
5. `SJMP NEXT`

They’re quick wins and make the system feel alive.

---

## **2. Memory access primitives**
These are essential for the dictionary and compiler.

- `@` (fetch 16‑bit from memory)
- `!` (store 16‑bit to memory)
- `C@` (fetch byte)
- `C!` (store byte)

These let high‑level Forth words manipulate memory, which is the foundation for:

- CREATE
- CONSTANT
- VARIABLE
- Colon definitions

Once these exist, the system can start building itself.

---

## **3. Literal handling**
You need a primitive that pushes a literal from the parameter field.

Traditionally:

- `LIT` (fetch next 16‑bit value from IP and push it)

This is the first primitive that **consumes data from the parameter field** instead of the stack.

It’s also the first primitive that modifies IP manually.

---

## **4. Dictionary creation code**
This is the next big milestone.

You need assembly routines that:

- Build a new dictionary header
- Link it to the previous word
- Store the name length/flags
- Store the name characters
- Store the CFA
- Set HERE to the start of the parameter field

This is where your dictionary format becomes real.

This code lives in `dictionary.asm`.

---

## **5. CREATE, DOES>, CONSTANT, VARIABLE**
These are the “meta‑words” that let Forth define new words at runtime.

They rely on:

- Memory access primitives
- Dictionary creation routines
- LIT
- EXIT

Once these exist, you can define new words *in Forth itself*.

---

## **6. Colon compiler (`:` and `;`)**
This is the moment your system becomes a real Forth.

`:` does:

- Parse a name
- Create a dictionary header
- Store CFA = CFA of DOCOL (the codeword for high‑level words)
- Switch to compile mode

`;` does:

- Compile EXIT into the parameter field
- Switch back to interpret mode
- Mark the word as complete

After this, you can write:

```
: SQUARE  DUP * ;
```

and your system will compile and run it.

---

## **7. The interpreter**
This is the outer loop:

- Read a token
- Search dictionary
- If found:
  - Execute or compile CFA
- If not found:
  - Try to parse as number
  - If number, compile LIT + value or push it
- If not a number: error

This is the final piece that makes the system interactive.

---

# 🧭 Recommended next step for *you* right now

Given where you are, the next assembly you should write is:

## **The arithmetic primitives (`+`, `-`) and memory primitives (`@`, `!`)**

Why?

- They’re small
- They’re easy to test
- They unlock the dictionary creation code
- They’re required before you can implement `:` and `;`

You’ll feel the system “come alive” once these exist.

If you want, I can write the exact assembly for `+` and `@` next, in the same clean style as DUP/SWAP/OVER.
