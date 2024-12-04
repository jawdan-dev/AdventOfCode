#include <common/common.h>
#include <common/point.h>

#define MAP_SIZE 256
static char map[MAP_SIZE][MAP_SIZE];
static int width = 0, height = 0;

const bool checkWordDirection(const char* word, const int x, const int y, const int dx, const int dy) {
	const int wordLength = strlen(word);

	// Bounds check.
	const int
		mx = x + (dx * (wordLength - 1)),
		my = y + (dy * (wordLength - 1));
	if (x < 0 || y < 0 || x >= width || y >= height ||
		mx < 0 || my < 0 || mx >= width || my >= height) return false;

	// Find word.
	int ix = x, iy = y;
	for (int i = 0; i < wordLength; i++) {
		if (map[iy][ix] != word[i]) return false;

		ix += dx;
		iy += dy;
	}
	return true;
}


int main() {
	printf("\n\n\n");
	FileReader fileReader = FileReader_getInput();

	while (!fileReader.endReached) {
		FileReader_readUntilDelim(&fileReader, "\n");

		// Read map.
		width = fileReader.readBufferLength;
		strncpy(map[height++], fileReader.readBuffer, width);
	}

	const char* wordTarget = "XMAS";
	const Point checks[] = {
		{.x = +1, .y = +0 }, {.x = -1, .y = +0 },
		{.x = +0, .y = +1 }, {.x = +0, .y = -1 },
		{.x = +1, .y = +1 }, {.x = -1, .y = +1 },
		{.x = +1, .y = -1 }, {.x = -1, .y = -1 },
	};

	// Find word in all check directions.
	int targetCount = 0;
	for (int x = 0; x < width; x++)
		for (int y = 0; y < height; y++)
			for (int i = 0; i < ARRAY_SIZE(checks); i++)
				if (checkWordDirection(wordTarget, x, y, checks[i].x, checks[i].y))
					targetCount++;

	printf("Found %i\n", targetCount);
}