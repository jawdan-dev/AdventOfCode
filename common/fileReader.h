#pragma once
#include <common/common.h>
#include <stdio.h>

typedef struct {
	FILE* file;
	bool endReached;

	char* readBuffer;
	size_t readBufferLength, readBufferCapacity;

	size_t partialReadIndex;
} FileReader;

void FileReader_constructor(FileReader* const fileReader, const char* filePath) {
	printf("\n\n\n"); // Sorry to all the old submissions.

	fileReader->file = fopen(filePath, "rb");
	ASSERT(fileReader->file, "Failed to open file reader.");

	fileReader->readBuffer = nullptr;
	fileReader->readBufferLength = 0;
	fileReader->readBufferCapacity = 0;
	fileReader->endReached = false;

	fileReader->partialReadIndex = 0;
}
const FileReader FileReader_getInput() {
	FileReader fileReader;
	FileReader_constructor(&fileReader, "./input.txt");
	return fileReader;
}

void FileReader_readUntilDelim(FileReader* const fileReader, const char* deliminators) {
	const size_t delimCount = strlen(deliminators);
	fileReader->readBufferLength = 0;

	char c;
	while (!feof(fileReader->file)) {
		c = fgetc(fileReader->file);
		if (c == EOF) {
			fileReader->endReached = true;
			break;
		}

		bool delimFound = false;
		for (size_t i = 0; i < delimCount && !delimFound; i++)
			if (c == deliminators[i])
				delimFound = true;
		if (delimFound) break;

		expandMemory((void**)&fileReader->readBuffer, fileReader->readBufferLength + 1, &fileReader->readBufferCapacity, sizeof(char));
		fileReader->readBuffer[fileReader->readBufferLength++] = c;
	}

	// End of buffer character.
	expandMemory((void**)&fileReader->readBuffer, fileReader->readBufferLength + 1, &fileReader->readBufferCapacity, sizeof(char));
	fileReader->readBuffer[fileReader->readBufferLength] = '\0';

	// Reset partial read head.
	fileReader->partialReadIndex = 0;
}

#define FileReader_readINTTYPE(name, type) \
const bool FileReader_readNext ## name(FileReader* const fileReader, type* const out) { \
	while (fileReader->partialReadIndex < fileReader->readBufferLength && \
		!('0' <= fileReader->readBuffer[fileReader->partialReadIndex] && \
			fileReader->readBuffer[fileReader->partialReadIndex] <= '9')) \
		fileReader->partialReadIndex++; \
	if (fileReader->partialReadIndex >= fileReader->readBufferLength) return false; \
	*out = 0; \
	while (fileReader->partialReadIndex < fileReader->readBufferLength && \
			'0' <= fileReader->readBuffer[fileReader->partialReadIndex] && \
				fileReader->readBuffer[fileReader->partialReadIndex] <= '9') { \
		*out *= 10; \
		*out += fileReader->readBuffer[fileReader->partialReadIndex] - '0'; \
		fileReader->partialReadIndex++; \
	} \
	return true; \
}

FileReader_readINTTYPE(Int, int);
FileReader_readINTTYPE(UInt, unsigned int);
FileReader_readINTTYPE(LLI, long long);
FileReader_readINTTYPE(LLU, unsigned long long);

#undef FileReader_readINTTYPE

void FileReader_readMap(FileReader* const fileReader, char* map, const int maxWidth, const int maxHeight, int* const width, int* const height) {
	*width = 0;
	*height = 0;

	while (!fileReader->endReached) {
		// Read map line.
		FileReader_readUntilDelim(fileReader, "\n");

		// Update dimension.
		*width = fileReader->readBufferLength;
		ASSERT(*width <= maxWidth, "Map width exceeded.");
		ASSERT(*height < maxHeight, "Map height exceeded.");

		// Move line into memory.
		for (int i = 0; i < *width; i++)
			map[(i * maxHeight) + *height] = fileReader->readBuffer[i];

		// Update dimension.
		(*height)++;
	}
}