# AX.25 Decoder - Host-Based Test Harness

This directory contains a host-based test environment for the AX.25 decoder, allowing you to debug and validate the decoder logic on your development machine before testing on the STM32F030 target.

## Files

### Core Test Files
- **input-wav.c** - Main test program with WAV file reader and decoder pipeline
- **Makefile-host** - Build configuration for x86/Linux host compilation

### Hardware Stub Implementations
- **serial_stub.c** - Stub for serial UART (output goes to stdout)
- **adc_stub.c** - Stub for ADC/DMA (dummy implementations)
- **buttons_stub.c** - Stub for button input (dummy implementations)

## Building

```bash
cd test
make -f Makefile-host
```

This creates an executable `ax25-decoder-host` that can be run on your development machine.

## Usage

```bash
./ax25-decoder-host input_audio.wav
```

### Requirements
- Input file must be a valid WAV file
- Supported audio formats:
  - PCM (uncompressed)
  - 8-bit or 16-bit samples
  - Mono audio only
  - Any sample rate (will be read from WAV header)

### Example Output

```
=== AX.25 Decoder Test Harness ===
Reading: example.wav

RIFF file size: 44544 bytes
Audio format: 1 (1=PCM)
Channels: 1
Sample rate: 9600 Hz
Bits per sample: 16
Audio data size: 44512 bytes

Initializing decoders...
Sample rate: 9600 Hz
Block size: 64 samples
Processing audio...

=== FRAME 1 ===
Length: 42 bytes
Hex: 7E 82 A0 9E 8E 9A 88 40 40 96 8E 8A 8C 84 8E A8 60 ...
CRC calculated: 0x1234
CRC received:   0x1234
CRC: VALID

=== Test Summary ===
Total samples processed: 44512
Frames found: 1
Frames with valid CRC: 1
Success rate: 100.0%
```

## How It Works

### Architecture

```
WAV File
   |
   v
[input-wav.c: read samples from file]
   |
   v
[FSK Decoder: Goertzel filters detect 1200/2200 Hz tones]
   |
   v [outputs symbols: 0 or 1]
   v
[Bit Decoder: NRZI decoding converts symbols to bits]
   |
   v [outputs bits: 0 or 1]
   v
[Frame Decoder: Handles frame sync, bit stuffing, FCS]
   |
   v [outputs complete frames]
   v
[Display results and validate CRC]
```

### Audio Sample Processing

1. WAV file is read as PCM samples
2. Samples are scaled to match ADC range (12-bit: 0-4095)
   - 16-bit input: shifted right by 4 bits
   - 8-bit input: shifted left by 4 bits
3. Samples accumulated into blocks (64 samples per block)
4. Each block processed by FSK decoder to detect mark/space tones

### FSK Detection

- **Mark tone**: 1200 Hz (binary 1)
- **Space tone**: 2200 Hz (binary 0)
- Uses Goertzel algorithm for efficient single-frequency detection

### Decoder Pipeline

The same decoder logic runs on both the host and embedded target:
- `decoder.c` - Frame assembly, bit stuffing, FCS validation
- `goertzel.c` - Tone detection (no hardware dependencies)
- `tnc2.c` - Frame formatting

## Advantages of Host Testing

1. **Fast iteration** - Recompile and test in seconds
2. **Easy debugging** - Use gdb, valgrind, printf debugging
3. **No embedded complexity** - No need to flash, debug over JTAG
4. **Test reproducibility** - Same WAV file always produces same results
5. **Create test suites** - Build multiple WAV files with known content

## Limitations

- Audio must be from a WAV file (not real-time ADC)
- Serial output goes to stdout (not actual UART)
- Timing is not cycle-accurate
- Button input not simulated

## Next Steps

1. Create test WAV files with recorded AX.25 packets
2. Run decoder and verify frame detection
3. Debug any issues with the decoder logic
4. Once working, test on the embedded target

## Debugging Tips

### Verbose Output
If you need more debug output, modify `input-wav.c` to print:
- Block-by-block FSK outputs
- Bit-by-bit decoder state
- Frame synchronization events

### Test Files
Create WAV files with known content:
- Single test packet for basic validation
- Multiple packets for sync/resync testing
- Edge cases: minimum/maximum frame sizes
- Corrupted packets for error handling

### Profiling
Use standard Linux tools:
```bash
gdb ./ax25-decoder-host
valgrind --leak-check=full ./ax25-decoder-host test.wav
time ./ax25-decoder-host test.wav
```

## Porting Back to Embedded

When moving back to the STM32F030:

1. Replace hardware stubs with real implementations:
   - `adc.c` - Real ADC/DMA driver
   - `serial.c` - Real UART driver
   - `buttons.c` - Real button input driver

2. The core decoder files remain unchanged:
   - `decoder.c`
   - `goertzel.c`
   - `tnc2.c`

3. The main embedded firmware uses the same decoder pipeline
