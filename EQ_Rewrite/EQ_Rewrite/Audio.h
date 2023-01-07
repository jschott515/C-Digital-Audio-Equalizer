#ifndef AUDIO_H
#define AUDIO_H

#include "defines.h"

void duplicateWAV(const char* file1, const char* file2);
Wav* parseWAV(FILE* fp);
void writeWavHead(FILE* fp, Wav* info);
void writeBuffer(FILE* fp, Wav* info, Sample* samples, int offset);

#endif // !AUDIO_H
