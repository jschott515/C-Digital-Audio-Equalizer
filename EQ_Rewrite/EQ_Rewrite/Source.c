#include "defines.h"

#include "Audio.h"
#include "Buffer.h"
#include "eq.h"
#include "FFT.h"
#include "IFFT.h"

void applyEQ();

int main(int argc, char** argv)
{
	duplicateWAV("snare1.wav", "snare2.wav");

	applyEQ();

	_CrtDumpMemoryLeaks();
}


void applyEQ()
{
	EQ* levels = malloc(sizeof(EQ) * EQ_BAND_COUNT);
	(void)initEQbands(levels);

	printf("\nEQ Bands:\n\n FREQ   \tGAIN\n------------------------\n");
	for (int i = 1; i < EQ_BAND_COUNT - 1; i++)
	{
		printf("%5d Hz\t%.1f dB\n", (levels + i)->frequencyBand, 20 * log10((levels + i)->scalarGain));
	}

	Complex* H = malloc(sizeof(Complex) * MAX_FREQ); // H(w) frequency response for 0kHz -> 32.768kHz (best size for radix-2 DFT)
	(void)generateEQfreqResp(levels, H);
	free(levels);

	FILE* fp1 = fopen("snare1.wav", "rb");
	FILE* fp2 = fopen("snare1_eq.wav", "wb");
	Wav* info = parseWAV(fp1);
	writeWavHead(fp2, info);

	int byte_width = info->sample_size / 8;
	int sample_count = info->data_size / byte_width;

	Buffer* audio = initBuffer();
	Complex* data = calloc(sizeof(Complex), MAX_FREQ);
	Sample* eq_data = malloc(sizeof(Sample) * BUFFER_SIZE);

	for (int i = 0; i < sample_count / BUFFER_SIZE; i++)
	{
		bufferBlock(audio, fp1, info, i * BUFFER_SIZE * byte_width);
		formatForFFT(audio, data);
		FFT(&data);

		for (int j = 0; j < MAX_FREQ; j++)
		{
			mulComplex((data + j), (H + j));
		}

		IFFT(&data, eq_data);
		writeBuffer(fp2, info, eq_data, (i * BUFFER_SIZE * byte_width));
		
	}
	freeBuffer(audio);
	free(data);
	free(eq_data);
	free(info);

	fclose(fp1);
	fclose(fp2);
}

