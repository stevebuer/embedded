#!/usr/bin/env python3
"""
Extract and analyze decoded bytes from AX.25 decoder output
"""

import sys

# Byte sequence from analyzing the wav file
# This should show us what the decoder sees
bytes_sequence = [
    0xFF, 0xFF, 0xFC, 0xFE, 0x73, 0xF9, 0xFC, 0xE7,
    0xE7, 0x9F, 0xCC, 0xFE, 0x7B, 0xE7, 0xCE, 0x7F,
    0x9F, 0xCF, 0xE7, 0xF3, 0x9E, 0xFC, 0xFE, 0x7F,
    0x3F, 0x9E, 0x53, 0x01
]

print("AX.25 Frame Sync Analysis")
print("=" * 50)
print()

# Look for frame sync byte (0x7E)
flag_byte = 0x7E
print(f"Searching for frame sync byte: 0x7E (01111110)")
print()

found = False
for i, byte in enumerate(bytes_sequence):
    if byte == flag_byte:
        print(f"✓ FOUND 0x7E at position {i} (byte {i})")
        found = True
        
        # Show context
        start = max(0, i-2)
        end = min(len(bytes_sequence), i+3)
        print(f"  Context: ", end="")
        for j in range(start, end):
            if j == i:
                print(f"[0x{bytes_sequence[j]:02X}]", end=" ")
            else:
                print(f"0x{bytes_sequence[j]:02X}", end=" ")
        print()
        print()

if not found:
    print("✗ NO 0x7E flag byte found!")
    print()
    print("Byte sequence:")
    for i, byte in enumerate(bytes_sequence):
        if (i + 1) % 8 == 0:
            print(f"0x{byte:02X}")
        else:
            print(f"0x{byte:02X}", end=" ")
    print()
    print()
    print("This suggests:")
    print("1. direwolf is not generating standard AX.25 frames")
    print("2. The audio encoding is not Bell 202 FSK (1200/2200 Hz)")
    print("3. Bit order or NRZI decoding may be inverted")
    print("4. Sample rate mismatch between encoding and decoder")
    print()
    print("Recommendations:")
    print("- Check direwolf's mode with: direwolf -h | grep gen_packets")
    print("- Try: direwolf -a 44100 gen_packets -o test.wav")
    print("  (specify sample rate of 44100)")
    print("- Or use: arecord -r 9600 -f S16_LE recorded.wav")
    print("  (record actual AX.25 at 9600 Hz)")
