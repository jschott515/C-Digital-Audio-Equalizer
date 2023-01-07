#ifndef EQ_H
#define EQ_H

#include "defines.h"

int initEQbands(EQ* levels); // Initialize EQ struct array default
int generateEQfreqResp(EQ* levels, Complex* freqRespH); // Generate complete frequency response data based on EQ bands
int extrapolate_linear(int x_diff, float y_diff, float y_offset, Complex* result); // Populate frequency response result array for data points A and B using linear extrapolation


#endif // !EQ_H