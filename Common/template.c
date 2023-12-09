// Load FileBuffer.
#define FILEBUFFER_READBUFFERSIZE 100
enum ReadState {
	ReadState_Line,
};
#include "../Common/FileBuffer.h"

// Entry point.
int main() {
	// Create file buffer.
	struct FileBuffer fileBuffer;
	FileBuffer_OpenFile(&fileBuffer, "input.txt");

	// File delims.
	struct FileDelim delims[] = {
		{ '\n', ReadState_Line }
	};
	FileBuffer_SetDelims(&fileBuffer, delims, sizeof(delims) / sizeof(*delims), ReadState_Line);

	// Interpretation variables.
	// TODO:

	// Interpret file.
	while (FileBuffer_ReadUntilNextDelim(&fileBuffer)) {
		// Processing.
		switch (fileBuffer.readState) {
			// TODO:
			
		}
	}

	// Output answer.
	printf("Output: %zu\n", 0);
	return 0;
}