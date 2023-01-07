#ifndef FFT_H
#define FFT_H

#include "defines.h"

void FFT(Complex** x);	// Fast Fourier Transform of the Time Domain Audio signal to produce the Frequency Response Spectrum
Complex* initFFTMultiplier(); // Creates array with values W_N ^ nk = e^-j(2nkpi/N) = cos(-2nkpi/N) + sin(-2nkpi/N)j for N/2 number of frequencies
void executeFFTStage(int task_size, Complex* x, Complex* result, Complex* mul);
void mulComplex(Complex* dest, Complex* mul); // Multiplies complex numbers mul and dest and places the result in dest

#endif // !FFT_H
