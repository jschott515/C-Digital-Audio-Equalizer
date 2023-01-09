#ifndef DEFINES_H
#define DEFINES_H

#define _CRT_SECURE_NO_WARNINGS
#define _CRTDBG_MAP_ALLOC
#define _USE_MATH_DEFINES

#include <stdio.h>
#include <stdint.h>
#include <math.h>


#define K 8192 // Size of FFT/IFFT - Yields ~2.7 Frequency Resolution for 22.05kHz audio
#define BUFFER_SIZE 1024 // Size of data block to load into buffer
#define MAX_FREQ 24000
#define EQ_BAND_COUNT 12
#define DATA_START 44 // Start of data chunk in standard .wav file format


typedef union SAMPLE {
	int data; // Uses union to make integer sample data byte addressable, used to format n-width input sample data to 4-byte integers 
	uint8_t buffer[4];
} Sample;


typedef struct BLOCK {
	Sample* samples; // Buffer linked list element, stores input blocks of size BUFFER_SIZE
	struct BLOCK* next, * prev;
} Block;


typedef struct BUFFER {
	Block* head; // Linked List type data structure for buffering input in blocks, used to implement shifting window
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
