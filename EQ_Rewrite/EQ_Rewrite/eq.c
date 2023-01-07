#include "eq.h"


int initEQbands(EQ* levels) // Initialize EQ struct array default
{
	levels->frequencyBand = 0; // 0Hz, 1 gain... Used to extrapolate first EQ band
	levels->scalarGain = 1;

	int baseFRQ = 32;
	//float vals[] = { 1.2, 1.2, 1.2, 1.2, 1.2, .001, .001, .001, .001, .001}; //bass boost
	float vals[] = { .001, .001, .001, .001, .001, 1.4, 1.4, 1.4, 1.4, 1.4 }; //treb boost
	for (int i = 1; i < EQ_BAND_COUNT - 1; i++)
	{
		(levels + i)->frequencyBand = baseFRQ;
		(levels + i)->scalarGain = vals[(i - 1)];
		baseFRQ *= 2;
	}

	(levels + EQ_BAND_COUNT - 1)->frequencyBand = MAX_FREQ; // Used to extrapolate last EQ band
	(levels + EQ_BAND_COUNT - 1)->scalarGain = 1;

	return 0;
}


int generateEQfreqResp(EQ* levels, Complex* freqRespH) // Generate complete frequency response data based on EQ bands
{
	int addr = 0;
	for (int i = 0; i < EQ_BAND_COUNT; i++)
	{
		int x_diff = (levels + i + 1)->frequencyBand - (levels + i)->frequencyBand;
		float y_diff = (levels + i + 1)->scalarGain - (levels + i)->scalarGain;
		extrapolate_linear(x_diff, y_diff, (levels + i)->scalarGain, (freqRespH + addr)); // Get EQ Band i to EQ Band i + 1...
		addr += x_diff;
	}

	return 0;
}


int extrapolate_linear(int x_diff, float y_diff, float y_offset, Complex* result) // Populate frequency response result array for data points A and B using linear extrapolation
{
	float slope = y_diff / x_diff;

	for (int i = 0; i < x_diff; i++) // Investigate placing eq bands on unevenly divided intervals, account for remaineder in this division.... helps if all EQ bands are div. by resolution
	{
		Complex temp = { y_offset + (slope * i), 0 };
		*(result + i) = temp;
	}

	return 0;
}