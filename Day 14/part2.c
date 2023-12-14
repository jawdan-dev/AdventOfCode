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

#define PATTERN_MAX 100
#define PATTERN_THRESHOLD 10000
	size_t basePattern[PATTERN_MAX], basePatternCount = 0;
	size_t rivalPattern[PATTERN_MAX], rivalPatternCount = 0;

	// Move rocks.
	size_t sameCounter = 0;
	const size_t max = 1000000000;
	for (size_t n = 0; n < max; n++) {
		// North.
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
		// West.
		for (size_t j = 0; j < height; j++) {
			for (size_t i = 0; i < width; i++) {
				if (rocks[i][j] == RockType_Dynamic) {
					size_t k = i;
					for (; k > 0 && rocks[k - 1][j] == RockType_Ground; k--) {
						rocks[k][j] = RockType_Ground;
						rocks[k - 1][j] = RockType_Dynamic;
					}
				}
			}
		}
		// South.
		for (size_t i = 0; i < width; i++) {
			for (size_t j = height - 1; j < height; j--) {
				if (rocks[i][j] == RockType_Dynamic) {
					size_t k = j;
					for (; k + 1 < height && rocks[i][k + 1] == RockType_Ground; k++) {
						rocks[i][k] = RockType_Ground;
						rocks[i][k + 1] = RockType_Dynamic;
					}
				}
			}
		}
		// East.
		for (size_t j = 0; j < height; j++) {
			for (size_t i = width - 1; i < width; i--) {
				if (rocks[i][j] == RockType_Dynamic) {
					size_t k = i;
					for (; k + 1 < width && rocks[k + 1][j] == RockType_Ground; k++) {
						rocks[k][j] = RockType_Ground;
						rocks[k + 1][j] = RockType_Dynamic;
					}
				}
			}
		}

		// Pattern matching magic:
		if (n >= PATTERN_THRESHOLD) {
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

			if (basePatternCount < 10) {
				basePattern[basePatternCount] = totalLoad;
				basePatternCount++;
			} else {
				if (rivalPatternCount >= basePatternCount) {
					printf("\nmatched with count %zu\n", rivalPatternCount);

					size_t start = max - PATTERN_THRESHOLD - 1;
					const size_t factor = rivalPatternCount;

					while (start >= factor) {
						start -= factor;
					}

					printf("index: %zu\n", start);
					printf("pattern matched value: %zu\n", basePattern[start]);

					break;
					rivalPatternCount = 0;
				}

				rivalPattern[rivalPatternCount] = totalLoad;
				rivalPatternCount++;
				if (basePattern[rivalPatternCount - 1] != totalLoad) {
					for (size_t i = 0; i < rivalPatternCount; i++) {
						bool matching = true;
						for (size_t j = i; j < rivalPatternCount && matching; j++)
							if (basePattern[j] != rivalPattern[j])
								matching = false;

						if (!matching) {
							basePattern[basePatternCount] = rivalPattern[0];
							basePatternCount++;
							if (basePatternCount >= PATTERN_MAX) {
								for (size_t j = 1; j < basePatternCount; j++) {
									basePattern[j - 1] = basePattern[j];
								}
								basePatternCount--;
							}
							for (size_t j = 1; j < rivalPatternCount; j++) {
								rivalPattern[j - 1] = rivalPattern[j];
							}
							rivalPatternCount--;
						} else {
							break;
						}

					}
				}
			}
		}

		printf("%i / %zu\r", n, max);
	}

	return 0;
}