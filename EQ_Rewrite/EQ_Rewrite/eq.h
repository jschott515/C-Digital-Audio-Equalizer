#ifndef EQ_H
#define EQ_H

#include "defines.h"

EQ* initEQbands(); // Initialize EQ struct array default
Complex* generateEQfreqResp(int Fs, EQ* levels); // Generate complete frequency response data based on EQ bands and sampling frequency
void extrapolate_linear(int Fs, EQ* levels, Complex* result); // Extrapolate Frequency Bands to K-sized array result. Includes Aliasing region for frequencies > Nyquist Frequency

#endif // !EQ_H