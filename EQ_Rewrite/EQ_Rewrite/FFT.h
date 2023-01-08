#ifndef FFT_H
#define FFT_H

#include "defines.h"

void FFT(Complex** x, Complex* mul);	// Fast Fourier Transform of the Time Domain Audio signal to produce the Frequency Response Spectrum
Complex* initFFTMultiplier(); // Creates array with values W_N ^ nk = e^-j(2nkpi/N) = cos(-2nkpi/N) + sin(-2nkpi/N)j for N/2 number of frequencies
void executeFFTStage(int task_size, Complex* x, Complex* result, Complex* mul);

void IFFT(Complex** X, Sample* x, Complex* mul);	// Inverse Fast Fourier Transform of frequency spectra X to produce x(t) impulse response. Shifts result to place impulse in center of signal
Complex* initIFFTMultiplier(); // Creates array with values W_N ^ -nk = e^-j(2-nkpi/N) = cos(2nkpi/N) + sin(2nkpi/N)j for N/2 number of frequencies
void executeIFFTStage(int task_size, Complex* X, Complex* result, Complex* mul);

void mulComplex(Complex* dest, Complex* mul); // Multiplies complex numbers mul and dest and places the result in dest

#endif // !FFT_H
