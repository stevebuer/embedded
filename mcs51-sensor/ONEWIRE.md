# 8051 1-Wire Notes

- Use any port pin such as **P1.0**
- Use 4.7 kΩ pull‑up to VCC
- Configure the pin as quasi‑bidirectional (default on most 8051s). 
- To drive LOW → write 0 to the bit. 
- To release HIGH → write 1 (the pull‑up resistor pulls it high).

This mimics open‑drain behavior. 

## Core Timing Requirements

The 1‑Wire protocol uses microsecond‑scale timing:

| Operation | Timing |
|----------|--------|
| Reset low | ≥480 µs |
| Presence detect sample | 60–70 µs after release |
| Write‑1 low | 1–15 µs |
| Write‑0 low | ≥60 µs |
| Read slot low | 1–15 µs |
| Read sample | ~15 µs after release |

These can be achieved with calibrated delay loops

## Delay routine

- 1 machine cycle = 1 µs 
- Most instructions = 1 cycle 

## Practical Tips for 8051 Implementation

- Disable interrupts during bit‑level operations to maintain timing
- Calibrate delay loop
- Some 8051 variants have open‑drain pins—use them if available
- For long cables, consider strong pull‑up or active pull‑up circuits
- If using DS18B20 sensors, they may need strong pull‑up during temperature conversion if powered parasitically

## To Do List

* Calibrate delay loop
