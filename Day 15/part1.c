// Load FileBuffer.
#define FILEBUFFER_READBUFFERSIZE 100
enum ReadState {
	ReadState_Line,
};
#include "../Common/FileBuffer.h"

const size_t hash(const char* str, const size_t len);

// Entry point.
int main() {
	// Create file buffer.
	struct FileBuffer fileBuffer;
	FileBuffer_OpenFile(&fileBuffer, "input.txt");

	// File delims.
	struct FileDelim delims[] = {
		{ ',', ReadState_Line }
	};
	FileBuffer_SetDelims(&fileBuffer, delims, sizeof(delims) / sizeof(*delims), ReadState_Line);

	// Interpretation variables.
	size_t total = 0;

	// Interpret file.
	while (FileBuffer_ReadUntilNextDelim(&fileBuffer)) {
		// Processing.
		switch (fileBuffer.readState) {
			case ReadState_Line: {
				// Hash and add to total.
				total += hash(fileBuffer.readBuffer, strlen(fileBuffer.readBuffer));
			} break;
		}
	}

	// Output answer.
	printf("Output: %zu\n", total);
	return 0;
}

const size_t hash(const char* str, const size_t len) {
	size_t hash = 0;
	for (size_t i = 0; i < len; i++) {
		hash += (size_t)str[i];
		hash *= 17;
		hash %= 256;
	}
	return hash;
}