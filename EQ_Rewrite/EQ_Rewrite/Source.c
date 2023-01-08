#include "defines.h"

#include "Audio.h"
#include "Buffer.h"
#include "eq.h"
#include "FFT.h"

void applyEQ(const char* file1, const char* file2);

int main(int argc, char** argv)
{
	//duplicateWAV("StarWars60.wav", "StarWarsCopy.wav");

	applyEQ("StarWars.wav", "StarWars_eq.wav");

	_CrtDumpMemoryLeaks();
}


void applyEQ(const char* file1, const char* file2)
{
	EQ* levels = initEQbands();	// Initialize EQ

	Complex* H = generateEQfreqResp(levels); // H(w) frequency response for 0kHz -> 32.768kHz (best size for radix-2 DFT)
	free(levels);
	
	FILE* fp1 = fopen(file1, "rb"); // Open source file and destination file
	FILE* fp2 = fopen(file2, "wb");

	Wav* info = parseWAV(fp1); // Read .wav data from source file header and copy to destination file
	writeWavHead(fp2, info);

	int byte_width = info->sample_size / 8;
	int sample_count = info->data_size / byte_width;

	Buffer* audio = initBuffer(); // Buffer for incoming audio chunks/blocks
	Complex* data = calloc(sizeof(Complex), MAX_FREQ); // allocate memory for data and result
	Sample* eq_data = malloc(sizeof(Sample) * BUFFER_SIZE);

	Complex* FFTmul = initFFTMultiplier(); // Pre-evaluate W_N ^ +/-nk values for FFT/IFFT Computation
	Complex* IFFTmul = initIFFTMultiplier();
	
	for (int i = 0; i < (sample_count / BUFFER_SIZE); i++) // Compute convolution in n sized partitions
	{
		bufferBlock(audio, fp1, info, i * BUFFER_SIZE * byte_width); // Load next data block
		formatForFFT(audio, data); // Rearrange in reverse-bit order as complex numbers for FFT
		FFT(&data, FFTmul); // Compute FFT in array data

		for (int j = 0; j < MAX_FREQ; j++) // Multiply arrays H(n)X(n) for convolution
		{
			mulComplex((data + j), (H + j));
		}

		IFFT(&data, eq_data, IFFTmul); // Convert back to time domain and write to file
		writeBuffer(fp2, info, eq_data, (i * BUFFER_SIZE * byte_width));		
	}

	freeBuffer(audio);
	free(H);
	free(data);
	free(eq_data);
	free(FFTmul);
	free(IFFTmul);
	free(info);

	fclose(fp1);
	fclose(fp2);
}

