#include "FFT.h"


void FFT(Complex* x, Complex* mul) // Inverse Fast Fourier Transform of frequency spectra X to produce x(t). Shifts result to place impulse in center of signal
{
	for (int i = 2; i <= K; i *= 2) //  Stages of cascading DFT stage blocks
	{
		for (int j = 0; j < K / i; j++) // nlog(n) total dft multiplier blocks 
		{
			int offset = i * j;
			executeFFTStage(i, (x + offset), mul);
		}
	}

	return;
}


Complex* initFFTMultiplier() // Creates array with values W_N ^ -nk = e^-j(2-nkpi/N) = cos(2nkpi/N) + sin(2nkpi/N)j for N/2 number of frequencies 
{
	Complex* arr = malloc(sizeof(Complex) * (K / 2));
	for (int n = 0; n < K / 2; n++) // Populate data to avoid redundant multiplications
	{
		Complex temp = { cos(-2.0 * n * M_PI / K), sin(-2.0 * n * M_PI / K) };
		*(arr + n) = temp;
	}

	return arr;
}


void executeFFTStage(int task_size, Complex* x, Complex* mul)
{
	for (int i = 0; i < task_size / 2; i++)
		mulComplex((x + i + task_size / 2), (mul + (i * K / task_size)));

	for (int i = 0; i < task_size / 2; i++)
	{
		Complex add = { (x + i)->real + (x + i + task_size / 2)->real, (x + i)->imag + (x + i + task_size / 2)->imag };
		Complex sub = { (x + i)->real - (x + i + task_size / 2)->real, (x + i)->imag - (x + i + task_size / 2)->imag };

		*(x + i) = add;
		*(x + i + task_size / 2) = sub;
	}

	return;
}


void IFFT(Complex* X, Sample* x, Complex* mul) // Inverse Fast Fourier Transform of frequency spectra X to produce x(t)
{
	for (int i = K; i > 1; i /= 2) //  Stages of cascading DFT stage blocks
	{
		for (int j = 0; j < K / i; j++) // nlog(n) total dft multiplier blocks 
		{
			int offset = i * j;
			executeIFFTStage(i, (X + offset), mul);
		}
	}

	for (int i = 0; i < K; i++) // Reogranize calculated impulse response as real values in Sample array x, only keep valid bits (top block of X)
	{
		int index = reverseBits(i) + BUFFER_SIZE - K; // Translated and shifted index..
		if (index >= 0)
			x[index].data = (int)(X + i)->real;
	}

	return;
}


Complex* initIFFTMultiplier() // Creates array with values W_N ^ -nk = e^-j(2-nkpi/N) = cos(2nkpi/N) + sin(2nkpi/N)j for N/2 number of frequencies 
{
	Complex* arr = malloc(sizeof(Complex) * K / 2);
	for (int n = 0; n < K / 2; n++) // Populate data to avoid redundant multiplications
	{
		Complex temp = { cos(2.0 * n * M_PI / K), sin(2.0 * n * M_PI / K) };
		*(arr + n) = temp;
	}

	return arr;
}


void executeIFFTStage(int task_size, Complex* X, Complex* mul) // Probably want to modify this implementation to be a single function called once per stage...
{
	for (int i = 0; i < task_size / 2; i++)
	{
		Complex add = { (X + i)->real + (X + i + task_size / 2)->real, (X + i)->imag + (X + i + task_size / 2)->imag };
		Complex sub = { (X + i)->real - (X + i + task_size / 2)->real, (X + i)->imag - (X + i + task_size / 2)->imag };

		*(X + i) = add;
		*(X + i + task_size / 2) = sub;
	}
	if (task_size != 2) // All multiplications are by 1 for this case, ignore...
		for (int i = 0; i < task_size / 2; i++)
			mulComplex((X + i + task_size / 2), (mul + (i * K / task_size)));
	else
		for (int i = 0; i < task_size; i++)
			(X + i)->real /= K; // Final Step

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