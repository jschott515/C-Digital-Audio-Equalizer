#include "defines.h"

#include "Audio.h"
#include "Buffer.h"
#include "eq.h"
#include "FFT.h"


void applyEQ(const char* file1, const char* file2);


int main(int argc, char** argv)
{
	applyEQ("Snare.wav", "Snare_EQ.wav");
	applyEQ("StarWars.wav", "StarWars_EQ.wav"); // Note - 1:00 runtime, 22.05kHz Fs input file - Processed in ~31 Seconds with Frequency Resolution of ~2.7

	_CrtDumpMemoryLeaks();
}


void applyEQ(const char* file1, const char* file2)
{
	FILE* fp1 = fopen(file1, "rb"); // Open source file and destination file
	FILE* fp2 = fopen(file2, "wb");

	Wav* info = parseWAV(fp1); // Read .wav data from source file header and copy to destination file
	writeWavHead(fp2, info);

	EQ* levels = initEQbands();	// Initialize EQ

	Complex* H = generateEQfreqResp(info->Fs, levels); // H(w) frequency response for 0Hz -> Fs Hz
	free(levels);
	
	int byte_width = info->sample_size / 8;
	int sample_count = info->data_size / byte_width;

	Buffer* audio = initBuffer(); // Buffer for incoming audio chunks/blocks
	Complex* data = calloc(sizeof(Complex), K); // allocate memory for data and result
	Sample* eq_data = malloc(sizeof(Sample) * BUFFER_SIZE);

	Complex* FFTmul = initFFTMultiplier(); // Pre-evaluate W_N ^ +/-nk values for FFT/IFFT Computation
	Complex* IFFTmul = initIFFTMultiplier();
	
	for (int i = 0; i < (sample_count / BUFFER_SIZE); i++) // Compute convolution in n sized partitions
	{
		bufferBlock(audio, fp1, info); // Load next data block
		formatForFFT(audio, data); // Rearrange in reverse-bit order as complex numbers for FFT
		FFT(data, FFTmul); // Compute FFT in array data

		for (int j = 0; j < K; j++) // Multiply arrays H(n)X(n) for convolution
		{
			mulComplex((data + j), (H + j));
		}

		IFFT(data, eq_data, IFFTmul); // Convert back to time domain and write to file
		writeBuffer(fp2, info, eq_data);		
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

