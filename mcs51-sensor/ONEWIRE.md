# 8051 1-Wire Protocol Notes

* DQ is on P1.0 with 4.7k pull-up to VCC (5V)

## Core Timing Requirements

8051 instruction cycle is ~1.085 µs with 11.0592 Mhz crystal

## Reset and Presence

1. Master pulls low for 480 - 960 µs
2. Release to high and wait 15 - 60 µs
3. Slave pulls low for 60 - 240 µs
4. Wait minimum of 480 µs before next operation

Note: Presence is a logical OR for multi-drop bus.

## Bit Slots

### Write Slot (60 - 120 µs total)

* Write 1: master pulls low 1–15 µs, then releases for remainder of slot
* Write 0: master holds low for full slot duration (60–120 µs)

### Read Slot (60 - 120 µs total)

* Master pulls low ≥1 µs to initiate
* Master releases; slave drives bit value
* Master samples at ~15 µs from slot start
* Line released for remainder of slot

Recovery between slots: ≥1 µs minimum (may use more for margin)

## ROM Commands (device addressing)

* Read ROM (0x33): single-drop only, output device ROM id.
* Match ROM (0x55): this command is for *<your address here>*.
* Skip ROM (0xCC): single-drop only, this command is for any (meaning: the only) attached device.
* Search ROM (0xF0): walk the tree
* Alarm Search (0xEC): walk the tree, but only for alarm state devices

## Read / Write Bytes

1. Reset and Presence
2. ROM
3. Send 8 bits of command
4. Wait
5. Read 8 x N bits of data

## References



