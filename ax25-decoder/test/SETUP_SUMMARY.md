# AX.25 Decoder - Host Test Environment Summary

## What Was Built

A complete host-based testing environment for your AX.25 decoder that runs on x86-64 Linux, allowing you to:
- Test decoder logic without embedded hardware
- Debug issues in a forgiving environment  
- Create reproducible test cases with WAV files
- Iterate quickly without flash/debug cycles

## Files Created/Modified

### New Test Files
- **input-wav.c** - Main test harness with WAV file reader (265 lines)
- **serial_stub.c** - Serial I/O stub (outputs to stdout)
- **adc_stub.c** - ADC/DMA stub (dummy implementations)
- **buttons_stub.c** - Button input stub

### Build Configuration
- **Makefile-host** - Dedicated build config for host compilation
- **README-HOST.md** - Comprehensive documentation

### Documentation
- **QUICKSTART.sh** - Quick reference guide

## What Compiles

### Unchanged (100% compatible with x86)
- `decoder.c` - Frame assembly, bit stuffing, CRC validation
- `goertzel.c` - FSK tone detection (uses standard math library)
- `tnc2.c` - Frame formatting and output

**These are the critical files - they work identically on both host and target.**

### Hardware Abstraction
- **ADC**, **Serial**, **Buttons** - Stubbed out for testing
- The test harness reads samples from a WAV file instead of ADC
- Output goes to stdout instead of serial port

## Build Status

✅ **Successfully compiled to executable:** `ax25-decoder-host` (53 KB)

```
Type: ELF 64-bit LSB executable (x86-64)
Architecture: x86-64 (native host)
Debug info: Included (with -g flag for debugging)
```

## How to Use

### 1. Build
```bash
cd ax25-decoder/test
make -f Makefile-host
```

### 2. Run
```bash
./ax25-decoder-host <your_audio_file.wav>
```

### 3. Output
The program will:
- Parse WAV file header and validate format
- Process audio samples through FSK decoder
- Detect and validate AX.25 frames
- Display each frame as hex bytes and CRC validation
- Print summary statistics

## Next Steps

1. **Obtain test WAV files** - Record or create audio with AX.25 packets
2. **Run the decoder** - Process files and see if frames are detected
3. **Debug** - If no frames found, examine:
   - Sample rate matches encoding
   - Audio contains valid AX.25 content
   - Signal levels are in reasonable range
4. **Iterate** - Refine decoder parameters or fix bugs
5. **Port back** - Once working, the same core decoder runs on STM32F030

## Key Advantages

✅ **Fast iteration** - Recompile in seconds, test immediately  
✅ **No hardware needed** - Test with recorded audio files  
✅ **Full debugging** - Use gdb, valgrind, printf debugging  
✅ **Reproducible** - Same WAV file always produces same output  
✅ **Portable code** - Core decoder is pure C with no ARM specifics  

## What Still Runs on Hardware Only

- Real ADC sampling (1200 baud, 9600 Hz sample rate)
- UART communication
- Button input
- GPIO LED control
- DMA transfers

These will be handled by the real embedded firmware when you switch back.

## Architecture Overview

```
WAV File (mono PCM)
        ↓
[Host Test: input-wav.c]
        ↓
[Sample Buffer → Decoder Pipeline]
        ↓
[FSK Decoder: detect 1200/2200 Hz tones]
        ↓ (symbols)
[Bit Decoder: NRZI conversion]
        ↓ (bits)  
[Frame Decoder: sync, bit stuffing, FCS]
        ↓ (complete frames)
[Display and validate]
```

## Technical Details

### Audio Format Support
- 8-bit unsigned PCM
- 16-bit signed PCM  
- Any sample rate (read from WAV header)
- Mono only

### Sample Rate Handling
- Samples are scaled to match 12-bit ADC range (0-4095)
- 16-bit input: shifted right 4 bits
- 8-bit input: shifted left 4 bits
- Goertzel filter adapts to actual sample rate

### FSK Configuration
- Mark frequency: 1200 Hz (binary 1)
- Space frequency: 2200 Hz (binary 0)
- Block size: 64 samples per analysis

## Troubleshooting

If the decoder doesn't find frames:

1. **Check WAV file validity**
   - Open in Audacity to confirm audio is present
   - Verify mono, PCM format

2. **Verify sample rate**
   - Check the printed sample rate matches your audio
   - Decoder will work at any rate, but signals must be valid

3. **Check signal levels**
   - Waveform should have reasonable amplitude
   - Too quiet: FSK detection may fail
   - Too loud: May clip and distort

4. **Enable debug output**
   - Add printf statements to input-wav.c
   - Trace FSK output, bit decoder state, etc.

## Files Summary

```
test/
├── input-wav.c           [New] Main test program with WAV reader
├── serial_stub.c         [New] Serial output stub
├── adc_stub.c            [New] ADC/DMA stub
├── buttons_stub.c        [New] Button input stub
├── Makefile-host         [New] Host build configuration
├── README-HOST.md        [New] Detailed documentation
├── QUICKSTART.sh         [New] Quick reference
└── ax25-decoder-host     [New] Built executable
```

The core decoder files remain in `src/` and `inc/` - they're 100% portable.
