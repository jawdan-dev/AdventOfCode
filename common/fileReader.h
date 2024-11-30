#pragma once
#include <common/common.h>
#include <stdio.h>

typedef struct {
	FILE* file;
	bool endReached;

	char* readBuffer;
	size_t readBufferLength, readBufferCapacity;
} FileReader;

void FileReader_constructor(FileReader* const fileReader, const char* filePath) {
	fileReader->file = fopen(filePath, "rb");
	ASSERT(fileReader->file, "Failed to open file reader.");

	fileReader->readBuffer = nullptr;
	fileReader->readBufferLength = 0;
	fileReader->readBufferCapacity = 0;
	fileReader->endReached = false;
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
}