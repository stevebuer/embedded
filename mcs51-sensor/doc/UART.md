# MCS-51 UART Config

Registers for UART init and control.

**UART SFRs**

* PCON (0x87)
* SCON (0x98) 
* SBUF (0x99) 

**UART bit Registers**

* SM0/SM1 (0x9F/0x9E)
* REN (0x9C)
* TI (0x9A) 
* RI (0x99) 

**Related Timer 1 SFRs**

* TH1 (0x8D)
* TL1 (0x8B)

**Timer 1 bit registers**

## Register Detail

### PCON (Power Control)

Bit 7 doubles baudrate. 

### SCON (Serial Control) 0x98

Bit registers are 0x98 + bit number + 1

| Bit | Name | Function |
|-----|------|----------|
| 0   | RI   | RX interrupt flag |
| 1   | TI   | TX interrupt flag |
| 2   | RB8  | Receive bit 8 |
| 3   | TB8  | Transmit bit 8 |
| 4   | REN  | Receiver enable |
| 5   | SM2  | Multiprocessor enble |
| 6   | SM1  | Serial mode bit 1 (LSB) |
| 7   | SM0  | Serial mode bit 0 (MSB) |

* RI is set 1 when character is received.
* TI is set 1 when transmit is complete.

Both must be cleared in software.

#### UART Modes (SM0/SM1)

Mode 1: 0b0100 = 0x20

| SM0 | SM1 | Mode | Description |
|-----|-----|------|-------------|
| 0   | 0   | 0    | Shift register mode (8‑bit, fixed baud) |
| 0   | 1   | 1    | 8‑bit UART, variable baud (standard) |
| 1   | 0   | 2    | 9‑bit UART, fixed baud |
| 1   | 1   | 3    | 9‑bit UART, variable baud |

# Timer 1

Timer 1 is used for UART baud rate control in 8-bit auto-reload mode.

The reload value is calculated as:

TH1 = 256 - ((*Crystal Frequency Hz* / 384) / Baud)

e.g. TH1 = 256 - ((11059000 / 384) / 9600) = 253 = 0xFD

If PCON.7 is set then the baud rate is doubled and the formula is:

TH1 = 256 - ((*Crystal Frequency Hz*/ 192) / Baud)

### TMOD (0x89) — Timer Mode Register

| Bits | Name | Meaning |
|------|-------|---------|
| 7–4  | T1    | Timer 1 control bits |
| 3–0  | T0    | Timer 0 control bits |

### Timer 1 Bits (TMOD.0 - TMOD.7)

| Bit | Name | Meaning |
|-----|------|---------|
| 0/4 | M0   | Mode select bit 0 |
| 1/5 | M1   | Mode select bit 1 |
| 2/6 | C/T1  | 0 = Timer mode (counts machine cycles) |
| 3/7 | GATE1 | 0 = Timer runs regardless of INT1 |

## References

[8051 Serial Tutorial](https://www.8052mcu.com/tutser.phtml)
