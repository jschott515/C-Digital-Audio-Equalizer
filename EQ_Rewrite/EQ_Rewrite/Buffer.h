#ifndef BUFFER_H
#define BUFFER_H

#include "defines.h"

Buffer* initBuffer();
void printBuffer(Buffer* buffer);
void bufferBlock(Buffer* buffer, FILE* fp, Wav* info, int offset);
void formatForFFT(Buffer* buffer, Complex* x);
void freeBuffer(Buffer* buffer);

#endif