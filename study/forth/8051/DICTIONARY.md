# Forth dictionary overview

In a traditional indirect‑threaded Forth, the **dictionary** is just a linked list of word definitions stored in memory. Each entry in that list describes:

- The **name** of the word (what you type)
- How to **find** it (link to previous word)
- How to **execute** it (code field)
- Any **data or code** associated with it (parameter field)

The dictionary is both the **symbol table** and the **program image** of a Forth system.

---

## Dictionary entry layout

A classic ITC (Indirect Threaded Code) dictionary entry has this structure in memory:

1. **Link field** (2 bytes)  
2. **Name length + flags** (1 byte)  
3. **Name characters** (N bytes)  
4. **Code field address (CFA)** (2 bytes)  
5. **Parameter field** (0 or more bytes/words)

Visually:

```text
+------------------------+
| Link field (2 bytes)   | → pointer to previous word
+------------------------+
| Name length + flags    | → 5 bits length, 3 bits flags
+------------------------+
| Name characters (N)    | → ASCII, not zero-terminated
+------------------------+
| CFA (2 bytes)          | → how to execute this word
+------------------------+
| Parameter field (...)  | → data or threaded code
+------------------------+
```

Each new word is added to the **front** of this linked list, so the dictionary grows “downward” in memory and the most recently defined word is found first.

---

## Link field

**Purpose:** connect this word to the previous one in the dictionary.

- Size: 2 bytes (a 16‑bit address)
- Content: address of the previous dictionary entry, or `0` if this is the first word
- Effect: forms a singly linked list:

```text
LATEST → wordN → wordN-1 → ... → word1 → 0
```

When Forth searches for a word by name, it starts at `LATEST` and follows the link field chain until it finds a match or hits `0`.

---

## Name length and flags

**Purpose:** store the word’s name length and some status bits in a single byte.

- Size: 1 byte
- Low 5 bits: **name length** (0–31 characters)
- High 3 bits: **flags**, commonly:
  - **IMMEDIATE**: execute during compilation
  - **SMUDGE**: hide word during compilation
  - other system‑specific flags

Example:

- `00010101b`  
  - length = `10101b` = 21  
  - flags = `000b` (no flags)

- `11100100b`  
  - length = `00100b` = 4  
  - flags = `111b` (all three flag bits set)

This compact encoding lets the system quickly know how many bytes to skip to get past the name, and how to treat the word during compilation.

---

## Name characters

**Purpose:** store the actual name of the word as typed by the programmer.

- Size: N bytes (N = name length from the previous byte)
- Content: ASCII characters
- Not zero‑terminated; the length byte tells you how many characters to read

Example for the word `SWAP`:

```text
length/flags: 04h
name bytes:   53h 57h 41h 50h   ; "S" "W" "A" "P"
```

When searching the dictionary, Forth:

1. Reads the length/flags byte
2. Masks off the flags to get the length
3. Compares that many characters with the input name

If both length and characters match, the word is found.

---

## Code field address (CFA)

**Purpose:** tell Forth **how to execute** this word.

- Size: 2 bytes (a 16‑bit address or encoded index)
- Meaning depends on the implementation, but in ITC it usually points to:
  - A **codeword** for primitives (low‑level assembly routines), or
  - The **first parameter** for high‑level words (threaded code)

Two common patterns:

### 1. Primitives

For a primitive word (implemented in assembly):

- The CFA points to a **codeword** that eventually jumps into the primitive’s machine code.
- In some 8‑bit designs, the CFA may hold an **index** into a jump table of primitives instead of a raw address.

When NEXT fetches a CFA for a primitive, it uses it to dispatch to the correct assembly routine.

### 2. High‑level words

For a high‑level word (defined in Forth):

- The CFA points to the **start of the parameter field**, which is a list of CFAs of other words.
- NEXT uses this CFA as the starting IP (instruction pointer) for executing the word’s body.

In both cases, the CFA is the bridge between the **name** and the **execution behavior**.

---

## Parameter field

**Purpose:** hold the data or threaded code associated with the word.

The parameter field’s content depends on the word’s type:

### 1. High‑level colon definitions

For a word defined like:

```forth
: FOO  DUP + EXIT ;
```

The parameter field is a list of CFAs:

```text
CFA(DUP)
CFA(+)
CFA(EXIT)
```

Execution flow:

1. The CFA of `FOO` points to the first CFA in its parameter field.
2. NEXT uses IP to walk through these CFAs.
3. Each CFA is executed in turn.
4. `EXIT` restores the caller’s IP and returns to NEXT.

### 2. Constants

For a constant:

```forth
10 CONSTANT TEN
```

The parameter field might contain the literal value:

```text
000Ah   ; 16-bit value 10
```

The codeword for CONSTANT knows to fetch this value and push it on the stack when executed.

### 3. Variables

For a variable:

```forth
VARIABLE X
```

The parameter field might contain the address of the storage location, or the storage itself, depending on the implementation. The codeword for VARIABLE pushes that address when executed.

### 4. CREATEd words

`CREATE` builds a dictionary entry with a parameter field that can later be interpreted however you like (e.g., as a buffer, structure, device descriptor, etc.).

---

## How dictionary search works

When you type a word at the Forth prompt, the system:

1. Reads the input name.
2. Starts at `LATEST` (the most recently defined word).
3. For each dictionary entry:
   - Follows the **link field** to the previous word.
   - Reads the **length/flags** byte.
   - Compares the **length** and **name characters** with the input.
4. If a match is found:
   - The system gets the word’s CFA.
   - Depending on the current state (interpret/compile), it either executes the word or compiles its CFA into the current definition.
5. If no match is found:
   - The system may try to interpret the input as a number.
   - If that fails, it reports an error.

The linked list structure means newer definitions override older ones with the same name, which is a powerful feature for extending or patching the system.

---

## How execution works with CFA, NEXT, and EXIT

The core of ITC Forth is the interaction between:

- **CFA**: where to go
- **NEXT**: how to step through CFAs
- **EXIT**: how to return

### NEXT

Conceptually:

1. Use IP to fetch the next CFA from the parameter field.
2. Advance IP to the next CFA.
3. Use the CFA to find the code to execute (primitive or high‑level).
4. Jump to that code.

### EXIT

Conceptually:

1. Pop the saved IP from the return stack.
2. Resume execution at that IP via NEXT.

### Putting it together

For a high‑level word:

1. Its CFA points to its parameter field (a list of CFAs).
2. When called, the current IP is pushed on the return stack, and IP is set to the word’s parameter field.
3. NEXT walks through the CFAs in the parameter field.
4. When `EXIT` is encountered, it restores the caller’s IP and returns to NEXT.

For a primitive:

1. Its CFA leads directly (or indirectly via an index) to its machine code.
2. The primitive does its work.
3. It ends by jumping back to NEXT.

---

## Summary

The Forth dictionary is:

- A **linked list** of entries (via the link field)
- Each entry has:
  - A **name** (length/flags + characters)
  - A **CFA** (how to execute it)
  - A **parameter field** (data or threaded code)
- The interpreter:
  - **Searches** the dictionary by following link fields and comparing names
  - **Executes** words by following CFAs and using NEXT/EXIT
- High‑level words are just:
  - Names + CFAs + lists of other CFAs

Once you see the dictionary as a chain of these entries, with NEXT walking CFAs and EXIT unwinding calls, the whole Forth execution model becomes a simple, elegant loop over a compact in‑memory structure.