#ifndef IFFT_H
#define IFFT_H

#include "defines.h"

void IFFT(Complex** X, Sample* x);	// Inverse Fast Fourier Transform of frequency spectra X to produce x(t) impulse response. Shifts result to place impulse in center of signal
void initIFFTMultiplier(Complex* arr); // Creates array with values W_N ^ -nk = e^-j(2-nkpi/N) = cos(2nkpi/N) + sin(2nkpi/N)j for N/2 number of frequencies
void executeIFFTStage(int task_size, Complex* X, Complex* result, Complex* mul); // INCOMPLETE

#endif // !IFFT_H