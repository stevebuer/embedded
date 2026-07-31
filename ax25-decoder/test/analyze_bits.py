#!/usr/bin/env python3
"""
Analyze AX.25 bit pattern from decoder output
"""

# First 200 bits from the decoder
bits_str = "11111111111111111111110011111110011100111111100111111100111001111110011110011111110011001111111001111011111001111100111001111111100111111100111111100111111100111001111011111100111111100111111100111111100111100101001101"

print("Analyzing AX.25 bit pattern from decoder...")
print(f"Total bits: {len(bits_str)}")
print()

# Show as bytes
print("Bits grouped by 8 (as bytes):")
for i in range(0, len(bits_str), 8):
    byte_bits = bits_str[i:i+8]
    if len(byte_bits) == 8:
        byte_val = int(byte_bits, 2)
        print(f"  Bits {i:3d}-{i+7:3d}: {byte_bits} = 0x{byte_val:02X} ({byte_val:3d})")
    else:
        print(f"  Bits {i:3d}-{i+len(byte_bits)-1:3d}: {byte_bits}")

print()
print("Looking for 0x7E (01111110 - AX.25 frame flag)...")
print("Looking for 0xFF (11111111 - AX.25 preamble)...")
print()

# Check for flag bytes
flag_pattern = "01111110"
preamble_pattern = "11111111"

flag_pos = bits_str.find(flag_pattern)
if flag_pos >= 0:
    print(f"✓ Found 0x7E flag at bit position {flag_pos}")
else:
    print(f"✗ Did NOT find 0x7E (01111110) pattern")
    # Try inverted
    inv_flag = "10000001"
    if bits_str.find(inv_flag) >= 0:
        print(f"  (But found inverted: 10000001)")

preamble_pos = bits_str.find(preamble_pattern)
if preamble_pos >= 0:
    print(f"✓ Found 0xFF preamble at bit position {preamble_pos}")
else:
    print(f"✗ Did NOT find 0xFF (11111111) preamble")

print()
print("If the flag was NOT found, the issue may be:")
print("1. Bit order is reversed (LSB first vs MSB first)")
print("2. NRZI decoding is inverted")
print("3. Symbol mapping is backwards (mark=space, space=mark)")
