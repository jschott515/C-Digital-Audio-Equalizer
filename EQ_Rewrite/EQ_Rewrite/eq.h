#ifndef EQ_H
#define EQ_H

#include "defines.h"

EQ* initEQbands(); // Initialize EQ struct array default
Complex* generateEQfreqResp(EQ* levels); // Generate complete frequency response data based on EQ bands
int extrapolate_linear(int x_diff, float y_diff, float y_offset, Complex* result); // Populate frequency response result array for data points A and B using linear extrapolation

#endif // !EQ_H