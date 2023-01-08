#include "Audio.h"


void duplicateWAV(const char* file1, const char* file2)
{
	FILE* fp1 = fopen(file1, "rb");
	FILE* fp2 = fopen(file2, "wb");
	Wav* info = parseWAV(fp1);

	writeWavHead(fp2, info);

	fseek(fp1, DATA_START, SEEK_SET);
	fseek(fp2, DATA_START, SEEK_SET);

	int byte_width = info->sample_size / 8;
	int sample_count = info->data_size / byte_width;

	Sample* samples = malloc(sizeof(Sample) * BUFFER_SIZE);

	for (int i = 0; i < (sample_count / BUFFER_SIZE); i++)
	{
		bufferAudio(fp1, info, samples, (i * BUFFER_SIZE * byte_width));
		writeBuffer(fp2, info, samples, (i * BUFFER_SIZE * byte_width));
	}

	free(samples);
	free(info);

	fclose(fp1);
	fclose(fp2);
	return;
}


Wav* parseWAV(FILE* fp) // Populates Wav struct with data from file header
{
	Wav* info = malloc(sizeof(Wav));

	fseek(fp, 4, SEEK_SET);
	fread(&info->file_size, 4, 1, fp);
	fseek(fp, 20, SEEK_SET);
	fread(&info->fmt, 2, 1, fp);
	fread(&info->chn, 2, 1, fp);
	fread(&info->Fs, 4, 1, fp);
	fseek(fp, 34, SEEK_SET);
	fread(&info->sample_size, 2, 1, fp);
	fseek(fp, 40, SEEK_SET);
	fread(&info->data_size, 4, 1, fp);

	if (info->fmt != 1) // Assert PCM Data Type
		return 1;
	return info;
}


void writeWavHead(FILE* fp, Wav* info) // Writes standard Wav header format to file for given info, assumes complete file size is known
{
	int fmtSize = 16;
	int byteRate = (info->Fs * info->sample_size * info->chn) / 8;
	short blockAlign = (info->sample_size * info->chn) / 8;

	fwrite("RIFF", 4, 1, fp);
	fwrite(&info->file_size, 1, 4, fp);
	fwrite("WAVE", 4, 1, fp);
	fwrite("fmt ", 4, 1, fp);
	fwrite(&fmtSize, 4, 1, fp);
	fwrite(&info->fmt, 2, 1, fp);
	fwrite(&info->chn, 2, 1, fp);
	fwrite(&info->Fs, 4, 1, fp);
	fwrite(&byteRate, 4, 1, fp);
	fwrite(&blockAlign, 2, 1, fp);
	fwrite(&info->sample_size, 2, 1, fp);
	fwrite("data", 4, 1, fp);
	fwrite(&info->data_size, 4, 1, fp);

	return;
}


void bufferAudio(FILE* fp, Wav* info, Sample* samples, int offset)
{
	if (info->chn != 1)
	{
		printf("\nInvalid Channel Count\n");
		return;
	}

	fseek(fp, DATA_START + offset, SEEK_SET);

	int byte_width = info->sample_size / 8;

	for (int i = 0; i < BUFFER_SIZE; i++)
	{

		uint8_t byte = 0;
		for (int j = 0; j < byte_width; j++) // Read byte by byte into sample data
		{
			byte = fgetc(fp);
			(samples + i)->buffer[j] = byte;
		}
		for (int j = 0; j < (sizeof(Sample) - byte_width); j++) // Sign extend remaining bytes in sample
		{
			if (byte & 0x80)
				(samples + i)->buffer[j + byte_width] = 0xFF;
			else
				(samples + i)->buffer[j + byte_width] = 0;
		}
		
	}

	return;
}


void writeBuffer(FILE* fp, Wav* info, Sample* samples, int offset)
{
	if (info->chn != 1)
	{
		printf("\nInvalid Channel Count\n");
		return;
	}

	fseek(fp, DATA_START + offset, SEEK_SET);

	int byte_width = info->sample_size / 8;

	for (int i = 0; i < BUFFER_SIZE; i++)
	{
		for (int j = 0; j < byte_width; j++) // Write data 1 byte at a time, formats 4 byte buffer data to n byte sample data
		{
			(void)fputc((samples + i)->buffer[j], fp);
		}
	}

	return;
}