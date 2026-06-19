If you’re building an **ITC Forth**, the very first subroutine you implement is always the same, no matter the CPU:

# ⭐ The first subroutine to write is **NEXT**

Everything else in an ITC Forth depends on `NEXT`.  
It is the *engine* that drives the entire system.

Without `NEXT`, nothing can run — not `EXIT`, not `DUP`, not even a simple high‑level word.  
So it’s always the first thing you bring to life.

---

# 🧠 Why NEXT comes first

In ITC:

- High‑level words are lists of CFAs.
- `NEXT` fetches the next CFA.
- It follows the CFA to the codeword.
- It jumps to the primitive code.
- The primitive eventually returns to `NEXT`.

So `NEXT` is the dispatcher, scheduler, and interpreter all in one.

Once `NEXT` works, you can start adding primitives one by one and test them interactively.

---

# 🧩 What NEXT needs before you write it

You need to decide:

### **1. Where is IP (Instruction Pointer)?**
Most 8051 Forths keep IP in DPTR or in two bytes of internal RAM.

### **2. How do you fetch a 16‑bit CFA from code memory?**
On the 8051, that means using `MOVC A,@A+DPTR` twice.

### **3. How do you jump to the codeword?**
The 8051 doesn’t have a direct “jump to address in register,” so you typically use:

- A jump table, or
- `JMP @A+DPTR` with a carefully arranged dispatch vector

This is the trickiest part of the whole kernel.

---

# 🧱 The second subroutine you implement

After `NEXT`, the next essential primitive is:

# ⭐ **EXIT**

`EXIT` returns from a high‑level word back to `NEXT`.

In ITC, `EXIT`:

- Pops the return stack into IP
- Jumps back to `NEXT`

Once you have `NEXT` + `EXIT`, you can execute simple high‑level definitions.

---

# 🛠️ The third thing you implement

After `NEXT` and `EXIT`, you add:

# ⭐ **The stack primitives**
- `DUP`
- `DROP`
- `SWAP`
- `OVER`

These let you test the system and bootstrap more complex words.

---

# 🚀 If you want, we can build NEXT together

I can help you:

- Choose where IP lives  
- Sketch the exact 8051 instructions  
- Build a minimal jump table  
- Write a working NEXT loop  
- Add EXIT  
- Add the first few primitives  

Just tell me how your ASM files are organized (e.g., `kernel.asm`, `primitives.asm`, `dict.asm`, etc.), and we can start wiring up the core of your ITC Forth.