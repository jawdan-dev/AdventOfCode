// Load FileBuffer.
#define FILEBUFFER_READBUFFERSIZE 100
enum ReadState {
	ReadState_Line,
};
#include "../Common/FileBuffer.h"

enum RockType {
	RockType_Ground,
	RockType_Static,
	RockType_Dynamic,
};

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
	size_t width = 0, height = 0;
	enum RockType rocks[FILEBUFFER_READBUFFERSIZE][FILEBUFFER_READBUFFERSIZE];

	// Interpret file.
	while (FileBuffer_ReadUntilNextDelim(&fileBuffer)) {
		// Processing.
		switch (fileBuffer.readState) {
			case ReadState_Line: {
				if (width < strlen(fileBuffer.readBuffer))
					width = strlen(fileBuffer.readBuffer);
				for (size_t i = 0; i < strlen(fileBuffer.readBuffer); i++) {
					enum RockType rockType = RockType_Ground;
					switch (fileBuffer.readBuffer[i]) {
						case 'O': rockType = RockType_Dynamic; break;
						case '#': rockType = RockType_Static; break;
					}
					rocks[i][height] = rockType;
				}
				height++;
			} break;
		}
	}

	// Move rocks.
	for (size_t i = 0; i < width; i++) {
		for (size_t j = 0; j < height; j++) {
			if (rocks[i][j] == RockType_Dynamic) {
				size_t k = j;
				for (; k > 0 && rocks[i][k - 1] == RockType_Ground; k--) {
					rocks[i][k] = RockType_Ground;
					rocks[i][k - 1] = RockType_Dynamic;
				}
			}
		}
	}


	// Total.
	size_t totalLoad = 0;
	for (size_t j = 0; j < height; j++) {
		const size_t load = height - j;
		for (size_t i = 0; i < width; i++) {
			if (rocks[i][j] == RockType_Dynamic) {
				totalLoad += load;
			}
		}
	}

	// Output answer.
	printf("Output: %zu\n", totalLoad);
	return 0;
}