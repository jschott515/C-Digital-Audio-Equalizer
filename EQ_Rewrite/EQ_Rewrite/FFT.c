#include "FFT.h"


void FFT(Complex** x) // Inverse Fast Fourier Transform of frequency spectra X to produce x(t). Shifts result to place impulse in center of signal
{
	Complex* mul = initFFTMultiplier();

	// CALCULATE IFFT //////////////////////////////////////////////////////////
	int count = 0; // Track how many bits are needed for array index
	for (int i = 2; i <= MAX_FREQ; i *= 2) //  Stages of cascading DFT stage blocks
	{
		Complex* result = malloc(sizeof(Complex) * MAX_FREQ);
		for (int j = 0; j < MAX_FREQ / i; j++) // nlog(n) total dft multiplier blocks 
		{
			int offset = i * j;
			executeFFTStage(i, (*x + offset), (result + offset), mul);
		}
		free(*x); // Note - original frequency response data is lost
		*x = result; // shallow copy array
		count++;
	}
	/////////////////////////////////////////////////////////////////////////////

	free(mul);
	return;
}


Complex* initFFTMultiplier() // Creates array with values W_N ^ -nk = e^-j(2-nkpi/N) = cos(2nkpi/N) + sin(2nkpi/N)j for N/2 number of frequencies 
{
	Complex* arr = malloc(sizeof(Complex) * (MAX_FREQ / 2));
	for (int n = 0; n < MAX_FREQ / 2; n++) // Populate data to avoid redundant multiplications
	{
		Complex temp = { cos(-2.0 * n * M_PI / MAX_FREQ), sin(-2.0 * n * M_PI / MAX_FREQ) };
		*(arr + n) = temp;
	}

	return arr;
}


void executeFFTStage(int task_size, Complex* x, Complex* result, Complex* mul)
{
	for (int i = 0; i < task_size / 2; i++)
		mulComplex((x + i + task_size / 2), (mul + (i * MAX_FREQ / task_size)));

	for (int i = 0; i < task_size / 2; i++)
	{
		Complex add = { (x + i)->real + (x + i + task_size / 2)->real, (x + i)->imag + (x + i + task_size / 2)->imag };
		Complex sub = { (x + i)->real - (x + i + task_size / 2)->real, (x + i)->imag - (x + i + task_size / 2)->imag };

		*(result + i) = add;
		*(result + i + task_size / 2) = sub;
	}

	return;
}


void mulComplex(Complex* dest, Complex* mul) // Multiplies complex numbers mul and dest and places the result in dest
{
	float a = dest->real * mul->real;
	float b = dest->real * mul->imag;
	float c = dest->imag * mul->real;
	float d = dest->imag * mul->imag;

	dest->real = a - d;
	dest->imag = b + c;

	return;
}