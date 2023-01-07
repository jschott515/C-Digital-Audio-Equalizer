#ifndef DEFINES_H
#define DEFINES_H

#define _CRT_SECURE_NO_WARNINGS
#define _CRTDBG_MAP_ALLOC
#define _USE_MATH_DEFINES

#include <stdio.h>
#include <stdint.h>
#include <math.h>


#define MAX_FREQ 32768
#define EQ_BAND_COUNT 12
#define BUFFER_SIZE 8192//128
#define DATA_START 44


typedef union SAMPLE {
	int data;
	uint8_t buffer[4];
} Sample;


typedef struct BLOCK {
	Sample* samples;
	struct BLOCK* next, * prev;
} Block;


typedef struct BUFFER {
	Block* head;
	Block* tail;
} Buffer;


typedef struct WAV {
	int file_size;		// Size of full file
	short fmt;			// Must be PCM (1)
	short chn;			// Number of Channels
	int Fs;				// Sample Rate
	short sample_size;	// Size of sample data in bits
	int data_size;		// Size of data section
} Wav;


typedef struct COMPLEX_NUMBER {
	float real, imag;
} Complex;


typedef struct EQ {
	int frequencyBand; // Pairs of frequency bands + gain for EQ adjustment
	float scalarGain;
} EQ;


#endif // !DEFINES_H
