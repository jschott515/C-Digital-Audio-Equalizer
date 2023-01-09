#include "Buffer.h"


Buffer* initBuffer()
{
	Buffer* buffer = malloc(sizeof(Buffer));

	buffer->tail = malloc(sizeof(Block)); // Create Tail block
	buffer->tail->samples = calloc(sizeof(Sample), BUFFER_SIZE);
	buffer->tail->next = NULL;

	Block* current = buffer->tail;

	for (int i = 0; i < K / BUFFER_SIZE - 1; i++) // Fill Buffer with Zeros
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


void bufferBlock(Buffer* buffer, FILE* fp, Wav* info)
{
	Block* temp = malloc(sizeof(Block));
	temp->samples = malloc(sizeof(Sample) * BUFFER_SIZE);
	bufferAudio(fp, info, temp->samples); // Load in new audio block

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
	for (Block* current = buffer->tail; current != NULL; current = current->prev)
	{
		for (int j = 0; j < BUFFER_SIZE; j++)
		{
			int index = reverseBits(j + (i * BUFFER_SIZE));
			x[index].real = (float)current->samples[j].data;
			x[index].imag = 0;
		}
		i++;
	}
	return;
}


unsigned int reverseBits(unsigned int num) // reverse bits in n wide bitfield to produce IFFT index
{
	unsigned int reverse_num = 0;
	int i;
	for (i = 0; i < (int)log2(K); i++) {
		if ((num & (1 << i)))
			reverse_num |= 1 << (((int)log2(K) - 1) - i);
	}
	return reverse_num;
}