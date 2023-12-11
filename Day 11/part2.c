// Load FileBuffer.
#define FILEBUFFER_READBUFFERSIZE 300
enum ReadState {
	ReadState_Line,
};
#include "../Common/FileBuffer.h"

//
struct Galaxy {
	size_t x, y;
	size_t expansionX, expansionY;
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
	struct Galaxy galaxies[500];
	size_t galaxyCount = 0;

	size_t galaxyWidth = 0, galaxyHeight = 0;

	// Interpret file.
	while (FileBuffer_ReadUntilNextDelim(&fileBuffer)) {
		// Processing.
		switch (fileBuffer.readState) {
			case ReadState_Line: {
				for (size_t i = 0; i < strlen(fileBuffer.readBuffer); i++) {
					if (fileBuffer.readBuffer[i] != '#') continue;

					if (galaxyCount > sizeof(galaxies) / sizeof(*galaxies)) {
						fprintf(stderr, "Galaxy max count exceeded.\n");
						abort();
					}

					// Add galaxy.
					galaxies[galaxyCount].x = i;
					galaxies[galaxyCount].y = galaxyHeight;
					galaxyCount++;

					// Update bounds.
					if (galaxyWidth < i) {
						galaxyWidth = i;
					}
				}
				galaxyHeight++;
			} break;
		}
	}


	// Reset expansion.
	for (size_t j = 0; j < galaxyCount; j++) {
		galaxies[j].expansionX = 0;
		galaxies[j].expansionY = 0;
	}

	// Expand x.
	for (size_t i = 0; i < galaxyWidth; i++) {
		bool canExpand = true;
		for (size_t j = 0; j < galaxyCount && canExpand; j++)
			if (galaxies[j].x == i)
				canExpand = false;

		if (!canExpand)
			continue;

		// Expand.
		for (size_t j = 0; j < galaxyCount; j++)
			if (galaxies[j].x >= i)
				galaxies[j].expansionX++;
	}
	// Expand y.
	for (size_t i = 0; i < galaxyWidth; i++) {
		bool canExpand = true;
		for (size_t j = 0; j < galaxyCount && canExpand; j++)
			if (galaxies[j].y == i)
				canExpand = false;

		if (!canExpand)
			continue;

		// Expand.
		for (size_t j = 0; j < galaxyCount; j++)
			if (galaxies[j].y >= i)
				galaxies[j].expansionY++;
	}

	for (size_t j = 0; j < galaxyCount; j++) {
		galaxies[j].x += 999999 * galaxies[j].expansionX;
		galaxies[j].y += 999999 * galaxies[j].expansionY;
	}



	size_t totalDistance = 0;
	for (size_t i = 0; i < galaxyCount; i++) {
		for (size_t j = i + 1; j < galaxyCount; j++) {
			const size_t cx =
				galaxies[i].x >= galaxies[j].x ?
				galaxies[i].x - galaxies[j].x :
				galaxies[j].x - galaxies[i].x;
			const size_t cy =
				galaxies[i].y >= galaxies[j].y ?
				galaxies[i].y - galaxies[j].y :
				galaxies[j].y - galaxies[i].y;

			const size_t distance = cx + cy;

			totalDistance += distance;
			//printf("%zu vs %zu = %zu\n", i + 1, j + 1, distance);
		}
	}

	// Output answer.
	printf("Output: %zu\n", totalDistance);
	return 0;
}