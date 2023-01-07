#include "Buffer.h"

Buffer* initBuffer()
{
	Buffer* buffer = malloc(sizeof(Buffer));

	buffer->tail = malloc(sizeof(Block)); // Create Tail block
	buffer->tail->samples = calloc(sizeof(Sample), BUFFER_SIZE);
	buffer->tail->next = NULL;

	Block* current = buffer->tail;

	for (int i = 0; i < MAX_FREQ / BUFFER_SIZE - 1; i++) // Fill Buffer with Zeros
	{
		Block* temp = malloc(sizeof(Block));
		temp->samples = calloc(sizeof(Sample), BUFFER_SIZE);
		temp->next = current;
		current->prev = temp;
		current = temp;
	}

	buffer->head = current; // Set Head block
	buffer->head->prev = NULL;

	return buffer;
}

void printBuffer(Buffer* buffer)
{
	/*for (Block* current = buffer->head; current != NULL; current = current->next) // Print from Head
	{
		printf("Block - \n");
		for (int j = 0; j < BUFFER_SIZE; j++)
		{
			printf("%d\n", current->samples[j].data);
		}
	}*/

	for (Block* current = buffer->tail; current != NULL; current = current->prev) // Print from Tail
	{
		printf("Block - \n");
		for (int j = 0; j < BUFFER_SIZE; j++)
		{
			printf("%d\n", current->samples[j].data);
		}
	}
	return;
}


void bufferBlock(Buffer* buffer, FILE* fp, Wav* info, int offset)
{
	Block* temp = malloc(sizeof(Block));
	temp->samples = malloc(sizeof(Sample) * BUFFER_SIZE);
	bufferAudio(fp, info, temp->samples, offset); // Load in new audio block

	temp->prev = NULL; // Update Buffer Head
	temp->next = buffer->head;
	buffer->head->prev = temp;
	buffer->head = temp;

	Block* aux = buffer->tail; // Update Buffer Tail
	buffer->tail = aux->prev;
	buffer->tail->next = NULL;
	free(aux->samples); // Deallocate discarded sample block
	free(aux);
	
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


void freeBuffer(Buffer* buffer)
{
	Block* current = buffer->head;
	while (current != NULL) {
		Block* temp = current;
		current = current->next;
		free(temp->samples);
		free(temp);
	}
	free(buffer);
}


void formatForFFT(Buffer* buffer, Complex* x)
{
	int i = 0;
	for (Block* current = buffer->tail; current != NULL; current = current->prev) // Print from Tail
	{
		for (int j = 0; j < BUFFER_SIZE; j++)
		{
			x[reverseBits(j + (i * BUFFER_SIZE))].real = (float)current->samples[j].data;
			x[reverseBits(j + (i * BUFFER_SIZE))].imag = 0;
		}
		i++;
	}
	return x;
}


unsigned int reverseBits(unsigned int num) // reverse bits in n wide bitfield to produce IFFT index
{
	unsigned int reverse_num = 0;
	int i;
	for (i = 0; i < (int)log2(MAX_FREQ); i++) {
		if ((num & (1 << i)))
			reverse_num |= 1 << (((int)log2(MAX_FREQ) - 1) - i);
	}
	return reverse_num;
}