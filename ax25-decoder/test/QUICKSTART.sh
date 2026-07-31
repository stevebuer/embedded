#!/bin/bash
# Quick reference for building and testing the host decoder

echo "AX.25 Decoder Host Test - Quick Start"
echo "====================================="
echo ""

# Check if we're in the test directory
if [ ! -f "Makefile-host" ]; then
    echo "Error: Please run this from the test/ directory"
    exit 1
fi

echo "Build instructions:"
echo "  make -f Makefile-host              # Build the decoder"
echo "  make -f Makefile-host clean        # Clean build artifacts"
echo ""

echo "Usage:"
echo "  ./ax25-decoder-host <input.wav>   # Process a WAV file"
echo ""

echo "Example:"
echo "  ./ax25-decoder-host recording.wav"
echo ""

echo "WAV file requirements:"
echo "  - Format: PCM (uncompressed)"
echo "  - Channels: Mono (1 channel)"
echo "  - Sample depth: 8-bit or 16-bit"
echo "  - Sample rate: Any (will be read from file)"
echo ""

echo "Test file creation tips:"
echo "  - Use Audacity or similar to record/create test audio"
echo "  - Export as WAV (PCM, 16-bit, 9600 Hz recommended)"
echo "  - Include known AX.25 packet content for validation"
echo ""

if [ -f "ax25-decoder-host" ]; then
    echo "Status: Binary is built and ready!"
    echo "Binary size: $(du -h ax25-decoder-host | cut -f1)"
else
    echo "Status: Binary not found - run 'make -f Makefile-host' first"
fi
