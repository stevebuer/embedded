/*
 * Host-based AX.25 decoder test harness
 *
 * This program reads a .wav file containing audio of AX.25 packets
 * and feeds the samples into the decoder pipeline for testing on x86.
 *
 * Uses libsndfile for audio file I/O (simplifies WAV parsing)
 *
 * Usage: ./ax25-decoder-host <input.wav> [-v]
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sndfile.h>

/* Decoder headers */
#include "../inc/goertzel.h"
#include "../inc/decoder.h"
#include "../inc/adc.h"
#include "../inc/serial.h"
#include "../inc/buttons.h"

/* Forward declarations for functions defined in decoder.c */
extern uint16_t AX25_CalculateCRC(const uint8_t *data, uint16_t len);
extern uint8_t *frame_decoder_get_frame(frame_decoder_t *fd, uint16_t *length);

/* Constants */
#define FSK_BLOCK_SIZE 64
#define AUDIO_BUFFER_SIZE 4096

/* Global decoder state */
static fsk_decoder_t fsk_decoder;
static bit_decoder_t bit_decoder;
static frame_decoder_t frame_decoder;

/* Statistics */
static uint32_t total_samples_processed = 0;
static uint32_t frames_found = 0;
static uint32_t frames_with_valid_fcs = 0;
static uint32_t debug_mode = 0;  /* Set to 1 for verbose FSK output */
static uint32_t block_count = 0;
static uint32_t mark_detections = 0;
static uint32_t space_detections = 0;
static uint32_t sync_count = 0;  /* Count of frame sync detections */
static uint32_t bit_count = 0;
static uint8_t bit_pattern[200];  /* Store first 200 bits for analysis */
static uint32_t bits_to_frame_decoder = 0;  /* Count bits fed to frame decoder */

/**
 * Open audio file using libsndfile
 */
static SNDFILE *open_audio_file(const char *filename, SF_INFO *sfInfo)
{
	/* Initialize SF_INFO structure (required by libsndfile) */
	memset(sfInfo, 0, sizeof(*sfInfo));

	/* Open file for reading */
	SNDFILE *file = sf_open(filename, SFM_READ, sfInfo);
	if (!file) {
		fprintf(stderr, "Error: Failed to open audio file: %s\n", sf_strerror(NULL));
		return NULL;
	}

	/* Display audio format */
	printf("Audio file loaded:\n");
	printf("  Sample rate: %d Hz\n", sfInfo->samplerate);
	printf("  Channels: %d\n", sfInfo->channels);
	printf("  Frames: %ld\n", sfInfo->frames);

	/* Check for supported formats */
	if (sfInfo->channels != 1) {
		fprintf(stderr, "Error: Only mono audio (1 channel) supported\n");
		sf_close(file);
		return NULL;
	}

	if (sfInfo->samplerate < 8000 || sfInfo->samplerate > 48000) {
		fprintf(stderr, "Error: Sample rate must be between 8000-48000 Hz\n");
		sf_close(file);
		return NULL;
	}

	return file;
}

/**
 * Process a single audio sample through the decoder pipeline
 */
static void process_sample(uint16_t sample)
{
	/* Static block buffer for FSK processing */
	static uint16_t sample_block[FSK_BLOCK_SIZE];
	static uint16_t block_index = 0;

	/* Accumulate samples into a block */
	sample_block[block_index++] = sample;

	if (block_index >= FSK_BLOCK_SIZE) {
		/* Process the block through FSK decoder */
		uint8_t symbol = fsk_process(&fsk_decoder, sample_block, FSK_BLOCK_SIZE);

		/* Print FSK status periodically for debugging */
		if (debug_mode && (block_count % 32 == 0)) {
			printf("Block %u: Symbol=%u Mark=%.0f Space=%.0f Threshold=%.0f\n",
				block_count,
				symbol,
				fsk_decoder.mark.power,
				fsk_decoder.space.power,
				fsk_decoder.mark.threshold);
		}

		/* Count symbol detections */
		if (symbol == 1) mark_detections++;
		else space_detections++;
		block_count++;

		/* Process FSK symbol through bit decoder */
		int16_t bit = bit_decoder_process(&bit_decoder, symbol);

		if (bit >= 0) {
			/* Store first N bits for analysis */
			if (bit_count < 200) {
				bit_pattern[bit_count] = bit ? 1 : 0;
			}
			bit_count++;
			bits_to_frame_decoder++;

			/* Detailed trace of first 50 bits */
			if (debug_mode && bit_count <= 50) {
				printf("Bit %3u: symbol=%u decoded_bit=%u, frame_buf=0x%02X bits=%u\n",
					bit_count, symbol, bit, frame_decoder.bit_buffer, frame_decoder.bit_count);
			}

			/* Process bit through frame decoder */
			int8_t frame_status = frame_decoder_process(&frame_decoder, (uint8_t)bit);

			/* Debug: show frame decoder state changes */
			if (debug_mode && frame_decoder.state != STATE_SYNCING) {
				if (frame_decoder.state == STATE_IN_FRAME) {
					sync_count++;
					if (sync_count == 1) {
						printf("[FRAME SYNC DETECTED] Starting frame collection...\n");
					}
				}
			}

			if (frame_status == 1) {
				/* Frame complete */
				uint16_t frame_len = 0;
				uint8_t *frame_data = frame_decoder_get_frame(&frame_decoder, &frame_len);

				if (frame_data != NULL && frame_len > 0) {
					frames_found++;
					printf("\n=== FRAME %u ===\n", frames_found);
					printf("Length: %u bytes\n", frame_len);
					printf("Hex: ");
					for (uint16_t i = 0; i < frame_len && i < 160; i++) {
						printf("%02X ", frame_data[i]);
					}
					printf("\n");

					/* Validate FCS */
					uint16_t crc = AX25_CalculateCRC(frame_data, frame_len - 2);
					uint16_t rx_fcs = (frame_data[frame_len-1] << 8) | frame_data[frame_len-2];

					printf("CRC calculated: 0x%04X\n", crc);
					printf("CRC received:   0x%04X\n", rx_fcs);

					if (crc == rx_fcs) {
						printf("CRC: VALID\n");
						frames_with_valid_fcs++;
					} else {
						printf("CRC: INVALID\n");
					}

					/* Reset for next frame */
					frame_decoder_init(&frame_decoder);
				}
			}
		}

		/* Reset block buffer */
		block_index = 0;
		total_samples_processed += FSK_BLOCK_SIZE;
	}
}

/**
 * Main test program
 */
int main(int argc, char **argv)
{
	SNDFILE *file;
	SF_INFO sfInfo;
	float audio_buffer[AUDIO_BUFFER_SIZE];
	sf_count_t frames_read;

	if (argc < 2) {
		fprintf(stderr, "Usage: %s <input.wav> [-v]\n", argv[0]);
		fprintf(stderr, "  -v : Enable verbose debug output\n");
		return 1;
	}

	/* Check for debug mode (optional -v flag) */
	if (argc == 3 && strcmp(argv[2], "-v") == 0) {
		debug_mode = 1;
		printf("Debug mode: ENABLED\n\n");
	}

	printf("=== AX.25 Decoder Test Harness ===\n");
	printf("Reading: %s\n\n", argv[1]);

	/* Open audio file using libsndfile */
	file = open_audio_file(argv[1], &sfInfo);
	if (!file) {
		return 1;
	}

	printf("\nInitializing decoders...\n");

	/* Initialize FSK decoder */
	fsk_init(&fsk_decoder, sfInfo.samplerate, FSK_BLOCK_SIZE);

	/* Initialize bit decoder */
	bit_decoder_init(&bit_decoder);

	/* Initialize frame decoder */
	frame_decoder_init(&frame_decoder);

	printf("Sample rate: %d Hz\n", sfInfo.samplerate);
	printf("Block size: %u samples\n", FSK_BLOCK_SIZE);
	printf("Processing audio...\n\n");

	/* Process audio frames from libsndfile */
	while ((frames_read = sf_readf_float(file, audio_buffer, AUDIO_BUFFER_SIZE)) > 0) {
		for (sf_count_t i = 0; i < frames_read; i++) {
			/* Convert float sample (-1.0 to 1.0) to 12-bit ADC range (0-4095) */
			float normalized = audio_buffer[i];
			/* Clamp to [-1, 1] */
			if (normalized < -1.0f) normalized = -1.0f;
			if (normalized > 1.0f) normalized = 1.0f;
			/* Convert to 0-4095 range: (norm + 1) * 2047.5 */
			uint16_t sample = (uint16_t)((normalized + 1.0f) * 2047.5f);
			
			process_sample(sample);
		}
	}

	/* Close file */
	sf_close(file);

	/* Print summary */
	printf("\n=== Test Summary ===\n");
	printf("Total samples processed: %u\n", total_samples_processed);
	printf("Blocks processed: %u\n", block_count);
	printf("Mark detections (1200 Hz): %u\n", mark_detections);
	printf("Space detections (2200 Hz): %u\n", space_detections);
	printf("Bits decoded: %u\n", bit_count);
	printf("Bits to frame decoder: %u\n", bits_to_frame_decoder);
	printf("Frame syncs found: %u\n", sync_count);
	printf("Frame decoder state: %u (0=syncing, 1=in_frame, 2=complete, 3=error)\n", frame_decoder.state);
	printf("Flags seen: %u\n", frame_decoder.frame_flags_seen);
	printf("Frames found: %u\n", frames_found);
	printf("Frames with valid CRC: %u\n", frames_with_valid_fcs);

	if (frames_found > 0) {
		printf("Success rate: %.1f%%\n", (100.0 * frames_with_valid_fcs) / frames_found);
	}

	/* Show first bits for analysis */
	if (debug_mode && bit_count > 0) {
		printf("\nFirst 200 bits (looking for 0x7E = 01111110 pattern):\n");
		for (uint32_t i = 0; i < 200 && i < bit_count; i++) {
			printf("%u", bit_pattern[i]);
			if ((i + 1) % 8 == 0) printf(" ");
			if ((i + 1) % 64 == 0) printf("\n");
		}
		printf("\n");
		
		printf("\nFrame decoder details:\n");
		printf("  Current byte buffer: 0x%02X\n", frame_decoder.bit_buffer);
		printf("  Bits in current byte: %u/8\n", frame_decoder.bit_count);
		printf("  Current stuff count: %u\n", frame_decoder.stuff_count);
	}

	return 0;
}
