#include "IFFT.h"


void IFFT(Complex** X, Sample* x) // Inverse Fast Fourier Transform of frequency spectra X to produce x(t). Shifts result to place impulse in center of signal
{
	Complex* mul = malloc(sizeof(Complex) * MAX_FREQ / 2);; // Presolve Wn^k values for transform
	initIFFTMultiplier(mul);

	// CALCULATE IFFT //////////////////////////////////////////////////////////
	for (int i = MAX_FREQ; i > 1; i /= 2) //  Stages of cascading DFT stage blocks
	{
		Complex* result = malloc(sizeof(Complex) * MAX_FREQ);
		for (int j = 0; j < MAX_FREQ / i; j++) // nlog(n) total dft multiplier blocks 
		{
			int offset = i * j;
			executeIFFTStage(i, (*X + offset), (result + offset), mul);
		}
		free(*X); // Note - original frequency response data is lost
		*X = result; // shallow copy array
	}
	/////////////////////////////////////////////////////////////////////////////


	for (int i = 0; i < MAX_FREQ; i++) // Reogranize calculated impulse response as real values in float array x, shift by N/2
	{
		int index = reverseBits(i) + BUFFER_SIZE - MAX_FREQ; // Translated and shifted index..
		if (index >= 0) 
			x[index].data = (int)(*X + i)->real;
	}

	free(mul);
	return;
}


void initIFFTMultiplier(Complex* arr) // Creates array with values W_N ^ -nk = e^-j(2-nkpi/N) = cos(2nkpi/N) + sin(2nkpi/N)j for N/2 number of frequencies 
{
	for (int n = 0; n < MAX_FREQ / 2; n++) // Populate data to avoid redundant multiplications
	{
		Complex temp = { cos(2.0 * n * M_PI / MAX_FREQ), sin(2.0 * n * M_PI / MAX_FREQ) };
		*(arr + n) = temp;
	}

	return;
}


void executeIFFTStage(int task_size, Complex* X, Complex* result, Complex* mul) // Probably want to modify this implementation to be a single function called once per stage...
{
	for (int i = 0; i < task_size / 2; i++)
	{
		Complex add = { (X + i)->real + (X + i + task_size / 2)->real, (X + i)->imag + (X + i + task_size / 2)->imag };
		Complex sub = { (X + i)->real - (X + i + task_size / 2)->real, (X + i)->imag - (X + i + task_size / 2)->imag };

		*(result + i) = add;
		*(result + i + task_size / 2) = sub;
	}
	if (task_size != 2) // All multiplications are by 1 for this case, ignore...
		for (int i = 0; i < task_size / 2; i++)
			mulComplex((result + i + task_size / 2), (mul + (i * MAX_FREQ / task_size)));
	else
		for (int i = 0; i < task_size; i++)
			(result + i)->real /= MAX_FREQ; // Final Step

	return;
}