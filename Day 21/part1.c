// Load FileBuffer.
#define FILEBUFFER_READBUFFERSIZE 300
enum ReadState {
	ReadState_Line,
};
#include "../Common/FileBuffer.h"

#define TILESEARCH_MAX 10000

typedef enum Direction {
	Direction_North,
	Direction_East,
	Direction_South,
	Direction_West,
} Direction;

typedef enum TileType {
	TileType_Rock,
	TileType_Plot,
} TileType;

typedef struct Position {
	size_t x, y;
} Position;

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
	TileType tiles[FILEBUFFER_READBUFFERSIZE][FILEBUFFER_READBUFFERSIZE];
	size_t width = 0, height = 0;
	size_t startX = SIZE_MAX, startY = SIZE_MAX;


	// Interpret file.
	while (FileBuffer_ReadUntilNextDelim(&fileBuffer)) {
		// Processing.
		switch (fileBuffer.readState) {
			case ReadState_Line: {
				if (height >= FILEBUFFER_READBUFFERSIZE) {
					fprintf(stderr, "Max tile height exceeded.\n");
					abort();
				}

				// Get buffer size.
				const size_t bufferSize = strlen(fileBuffer.readBuffer);
				if (width < bufferSize)
					width = bufferSize;

				// Read tiles.
				for (size_t i = 0; i < bufferSize; i++) {
					// Get tile.
					TileType tile = TileType_Rock;
					switch (fileBuffer.readBuffer[i]) {
						case 'S': {
							startX = i;
							startY = height;
						} // Overflow into next case.
						case '.': tile = TileType_Plot; break;
					}

					// Set tile.
					tiles[i][height] = tile;
				}

				// Update height.
				height++;
			} break;
		}
	}

	// Initialize search tiles.
	Position searchTiles[TILESEARCH_MAX];
	size_t searchTileCount = 0;
	searchTiles[0] = (Position) { .x = startX, .y = startY };
	searchTileCount++;

	// Initialize tile states.
	bool tileFound[FILEBUFFER_READBUFFERSIZE][FILEBUFFER_READBUFFERSIZE];
	//for (size_t i = 0; i < width; i++) {
	//	for (size_t j = 0; j < height; j++) {
	//		tileFound[i][j] = i == startX && j == startY;
	//	}
	//}

	size_t lastSuccessfulStepCount;
	for (size_t s = 0; s < 64; s++) {
		const size_t initialSearchTileCount = searchTileCount;
		lastSuccessfulStepCount = 0;

		for (size_t i = 0; i < width; i++) {
			for (size_t j = 0; j < height; j++) {
				tileFound[i][j] = false;
			}
		}

		// Search.
		for (size_t i = 0; i < initialSearchTileCount; i++) {
			// Checks.
			const Direction checks[] = {
				Direction_North, Direction_East,
				Direction_South, Direction_West,
			};

			// Check.
			for (size_t j = 0; j < sizeof(checks) / sizeof(*checks); j++) {
				const Direction direction = checks[j];
				Position checkPosition = searchTiles[i];

				switch (direction) {
					case Direction_North: checkPosition.y--; break;
					case Direction_East: checkPosition.x++; break;
					case Direction_South: checkPosition.y++; break;
					case Direction_West: checkPosition.x--; break;
				}

				// Check if check in bounds.
				if (checkPosition.x >= width || checkPosition.y >= height) {
					continue;
				}
				// Ignore rocks.
				if (tiles[checkPosition.x][checkPosition.y] == TileType_Rock) {
					continue;
				}

				// Ignore if already checked.
				if (tileFound[checkPosition.x][checkPosition.y]) {
					continue;
				}

				// Update count.
				lastSuccessfulStepCount++;

				// Mark found.
				tileFound[checkPosition.x][checkPosition.y] = true;

				if (searchTileCount >= TILESEARCH_MAX) {
					fprintf(stderr, "Tile search max exceeded.\n");
					abort();
				}

				// Explore.
				searchTiles[searchTileCount++] =
					(Position) { .x = checkPosition.x, .y = checkPosition.y };

			}
		}

		// Remove already-searched searches.
		memmove(searchTiles, searchTiles + initialSearchTileCount, (searchTileCount - initialSearchTileCount) * sizeof(*searchTiles));
		searchTileCount -= initialSearchTileCount;
	}

	for (size_t j = 0; j < height; j++) {
		for (size_t i = 0; i < width; i++) {
			if (tiles[i][j] == TileType_Rock) printf(".");
			else if (tileFound[i][j]) printf("+");
			else printf(" ");
		}
		printf("\n");
	}


	// Sum total.
	size_t totalFound = 0;
	for (size_t i = 0; i < width; i++)
		for (size_t j = 0; j < height; j++)
			if (tileFound[i][j])
				totalFound++;


	// Output answer.
	printf("Tile total: %zu\n", totalFound);
	printf("Output: %zu\n", lastSuccessfulStepCount);
	return 0;
}