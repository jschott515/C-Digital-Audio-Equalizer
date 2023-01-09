#include "eq.h"


EQ* initEQbands() // Initialize EQ struct array default
{
	EQ* levels = malloc(sizeof(EQ) * EQ_BAND_COUNT);

	levels->frequencyBand = 0; // 0Hz, 1 gain... Used to extrapolate first EQ band
	levels->scalarGain = 1;

	int baseFRQ = 32;
	//float vals[] = { 1, 1, 1, 1, 1, .251, .251, .251, .251, .251}; //No treble
	//float vals[] = { .251, .251, .251, .251, .251, 1, 1, 1, 1, 1 }; //No bass
	//float vals[] = { 1.73, 1.73, 1.73, 1.73, 1.31, .88, .67, .44, .33, .26 }; //Full Bass Preset
	float vals[] = { .5, .63, .63, .63, .79, 1, 1.13, 1.88, 2.2, 2.0 }; //Full Treble Preset

	for (int i = 1; i < EQ_BAND_COUNT - 1; i++)
	{
		(levels + i)->frequencyBand = baseFRQ;
		(levels + i)->scalarGain = vals[(i - 1)];
		baseFRQ *= 2;
	}

	(levels + EQ_BAND_COUNT - 1)->frequencyBand = MAX_FREQ; // Used to extrapolate last EQ band
	(levels + EQ_BAND_COUNT - 1)->scalarGain = 1;

	return levels;
}


Complex* generateEQfreqResp(int Fs, EQ* levels) // Generate complete frequency response data based on EQ bands and sampling frequency
{
	Complex* freqRespH = malloc(sizeof(Complex) * K); // K-sized complex number array for frequency response 
	extrapolate_linear(Fs, levels, freqRespH);
	return freqRespH;
}


void extrapolate_linear(int Fs, EQ* levels, Complex* result) // Extrapolate Frequency Bands to K-sized array result. Includes Aliasing region for frequencies > Nyquist Frequency
{
	float resolution = Fs / (float)K; // Delta_f frequency resolution
	EQ* current = levels;
	EQ* next = levels + 1;
	float start_bin = 0;
	float slope = (next->scalarGain - current->scalarGain) / (float)(next->frequencyBand - current->frequencyBand); // m = (Y2 - Y1) / (X2 - X1)

	for (int n = 0; n < K / 2; n++) // For Frequencies 0 Hz -> Nyquist Frequency
	{
		float bin_frq = n * resolution; // Bin N = n * Delta_f
		if (bin_frq > next->frequencyBand) // Update values used to calculate slope
		{
			current = next;
			next = next + 1;
			start_bin = bin_frq;
			slope = (next->scalarGain - current->scalarGain) / (float)(next->frequencyBand - current->frequencyBand);
		}

		Complex temp = { (slope * (bin_frq - start_bin)) + current->scalarGain,0 }; // y = mx + b, imaginary component set to 0.
		result[n] = temp;
		result[K - 1 - n] = temp; // Aliasing region - Mirrored Response
	}
	return;
}