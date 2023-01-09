#ifndef AUDIO_H
#define AUDIO_H

#include "defines.h"

Wav* parseWAV(FILE* fp);
void writeWavHead(FILE* fp, Wav* info);
void bufferAudio(FILE* fp, Wav* info, Sample* samples);
void writeBuffer(FILE* fp, Wav* info, Sample* samples);

#endif // !AUDIO_H
