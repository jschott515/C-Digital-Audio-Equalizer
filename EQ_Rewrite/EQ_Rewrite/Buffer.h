#ifndef BUFFER_H
#define BUFFER_H

#include "defines.h"

Buffer* initBuffer();
void bufferBlock(Buffer* buffer, FILE* fp, Wav* info);
void formatForFFT(Buffer* buffer, Complex* x);
void freeBuffer(Buffer* buffer);

#endif