#include <common/common.h>
#include <common/point.h>

#define MAP_SIZE 256
static char map[MAP_SIZE][MAP_SIZE];
static int width = 0, height = 0;

const bool checkWordRing(const int x, const int y) {
	// Bounds check.
	const int mx = x + 2, my = y + 2;
	if (x < 0 || y < 0 || x >= width || y >= height ||
		mx < 0 || my < 0 || mx >= width || my >= height) return false;

	// Check if center correct.
	if (map[y + 1][x + 1] != 'A') return false;

	// Get rings.
	const char* targetRing = "MMSS";
	const char ring[4] = { map[y][x], map[y][x + 2], map[y + 2][x + 2], map[y + 2][x] };

	// Compare all possible ring rotations.
	for (int o = 0; o < ARRAY_SIZE(ring); o++) {
		bool found = true;
		for (int i = 0; i < ARRAY_SIZE(ring) && found; i++)
			if (ring[(i + o) % ARRAY_SIZE(ring)] != targetRing[i])
				found = false;
		// Valid ring found.
		if (found) return true;
	}
	// No correct ring found.
	return false;
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

	// Checking for x-mases.
	int targetCount = 0;
	for (int x = 0; x < width; x++)
		for (int y = 0; y < height; y++)
			if (checkWordRing(x, y))
				targetCount++;

	printf("Found %i\n", targetCount);
}