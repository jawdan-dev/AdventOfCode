// Load FileBuffer.
#define FILEBUFFER_READBUFFERSIZE 200
enum ReadState {
	ReadState_Line,
};
#include "../Common/FileBuffer.h"

#define SEARCHNODES_MAX 100
#define SEARCHPATH_MAX 5000
#define FOUNDNODES_MAX 2000

typedef enum Direction {
	Direction_North,
	Direction_East,
	Direction_South,
	Direction_West,
} Direction;

typedef enum PathTile {
	PathTile_Slope_North,
	PathTile_Slope_East,
	PathTile_Slope_South,
	PathTile_Slope_West,
	PathTile_Forest,
	PathTile_Path,
} PathTile;

typedef struct PathLocation {
	size_t x, y;
} PathLocation;

const size_t getDistance(const size_t x1, const size_t y1, const size_t x2, const size_t y2);

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
	PathTile tiles[FILEBUFFER_READBUFFERSIZE][FILEBUFFER_READBUFFERSIZE];
	size_t width = 0, height = 0;

	// Interpret file.
	while (FileBuffer_ReadUntilNextDelim(&fileBuffer)) {
		// Processing.
		switch (fileBuffer.readState) {
			case ReadState_Line: {
				// Error check.
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
					PathTile tile = PathTile_Forest;
					switch (fileBuffer.readBuffer[i]) {
						case '#': tile = PathTile_Forest; break;
						case '.': tile = PathTile_Path; break;
						case '>': tile = PathTile_Slope_East; break;
						case 'v': tile = PathTile_Slope_South; break;
					}

					// Set tile.
					tiles[i][height] = tile;
				}

				// Update height.
				height++;
			} break;
		}
	}

	// Find starting position.
	size_t startX = 0, startY = 0;
	while (startX < width && tiles[startX][startY] == PathTile_Forest) startX++;
	if (startX >= width) {
		fprintf(stderr, "Starting X not found.\n");
		abort();
	}

	// Find target position.
	size_t targetX = 0, targetY = height - 1;
	while (targetX < width && tiles[targetX][targetY] == PathTile_Forest) targetX++;
	if (targetX >= width) {
		fprintf(stderr, "target X not found.\n");
		abort();
	}

	// Search nodes.
	typedef struct SearchNode {
		size_t x, y;
		Direction direction;
		size_t distance, travelCost;

		PathLocation path[SEARCHPATH_MAX];
		size_t pathLength;
	} SearchNode;
	SearchNode* searchNodes = malloc(SEARCHNODES_MAX * sizeof(SearchNode));
	size_t searchNodeCount = 0;

	// Initial node.
	searchNodes[searchNodeCount++] = (SearchNode) {
		.direction = Direction_South,
		.x = startX, .y = startY,
		.distance = getDistance(startX, startY, targetX, targetY), .travelCost = 0,
		.pathLength = 0,
	};
	searchNodes[0].path[searchNodes[0].pathLength++] = (PathLocation) { .x = searchNodes[0].x, .y = searchNodes[0].y };

	// Remember found nodes.
	typedef struct FoundNode {
		size_t x, y, travelCost;
	} FoundNode;
	FoundNode foundNodes[FOUNDNODES_MAX];
	size_t foundNodeCount = 0;

	// Search.
	// NOTE: A* time.
	while (searchNodeCount > 0) {
		// Get furthest node.
		SearchNode* activeNode = nullptr;
		size_t activeH, activeI;
		for (size_t i = 0; i < searchNodeCount; i++) {
			SearchNode* const node = &searchNodes[i];
			// Get h score.
			const size_t h = node->travelCost + node->distance;
			// Compare to active.
			if (activeNode == nullptr || h > activeH) {
				activeNode = node;
				activeH = h;
				activeI = i;
			}
		}

		// Error check.
		if (activeNode == nullptr) break;

		// Check if in bounds.
		if (activeNode->x < width || activeNode->y < height) {
			// Check if target hit.
			if (activeNode->distance == 0) {
				// Error checks.
				if (foundNodeCount >= FOUNDNODES_MAX) {
					fprintf(stderr, "Found max exceeded.\n");
					abort();
				}
				foundNodes[foundNodeCount++] = (FoundNode) {
					.x = activeNode->x,
					.y = activeNode->y,
					.travelCost = activeNode->travelCost,
				};
			} else {
				// Explore.
				const Direction checks[] = {
					Direction_North, Direction_East,
					Direction_South, Direction_West
				};
				for (size_t c = 0; c < sizeof(checks) / sizeof(*checks); c++) {
					const Direction direction = checks[c];

					// Ignore moving backwards.
					if ((direction + 2) % 4 == activeNode->direction) continue;

					// Get x, y.
					size_t x = activeNode->x, y = activeNode->y;
					switch (direction) {
						case Direction_North: y--; break;
						case Direction_East: x++; break;
						case Direction_South: y++; break;
						case Direction_West: x--; break;
					}

					// Ignore if out of bounds.
					if (x >= width || y >= height) continue;

					// Ignore forests.
					if (tiles[x][y] == PathTile_Forest) continue;

					// If slope, only move in the correct direction.
					const PathTile activeTile = tiles[activeNode->x][activeNode->y];
					if (activeTile != PathTile_Path && activeTile != direction) continue;

					// Make sure we haven't stepped here before.
					bool herePreviously = false;
					for (size_t i = 0; !herePreviously && i < activeNode->pathLength; i++)
						if (activeNode->path[i].x == x && activeNode->path[i].y == y)
							herePreviously = true;
					if (herePreviously) continue;

					// Error checks.
					if (activeNode->pathLength >= SEARCHPATH_MAX) {
						fprintf(stderr, "Search path max exceeded.\n");
						abort();
					}
					if (searchNodeCount >= SEARCHNODES_MAX) {
						fprintf(stderr, "Search node max exceeded.\n");
						abort();
					}

					// Place new search node.
					searchNodes[searchNodeCount] = (SearchNode) {
						.x = x, .y = y,
						.direction = direction,
						.distance = getDistance(x, y, targetX, targetY), .travelCost = activeNode->travelCost + 1,
						.pathLength = activeNode->pathLength + 1,
					};
					memcpy(searchNodes[searchNodeCount].path, activeNode->path, activeNode->pathLength * sizeof(PathLocation));
					searchNodes[searchNodeCount].path[activeNode->pathLength] = (PathLocation) { .x = x, .y = y };
					searchNodeCount++;
				}
			}
		}

		// Remove from list.
		if (searchNodeCount > 1) searchNodes[activeI] = searchNodes[searchNodeCount - 1];
		searchNodeCount--;
	}

	bool sorted = false;
	while (!sorted) {
		sorted = true;
		for (size_t i = 1; i < foundNodeCount; i++) {
			if (foundNodes[i - 1].travelCost < foundNodes[i].travelCost) {
				// Swap.
				const FoundNode temp = foundNodes[i - 1];
				foundNodes[i - 1] = foundNodes[i];
				foundNodes[i] = temp;
				// Update details.
				sorted = false;
			}
		}
	}
	// Print found nodes.
	printf("Found paths:\n");
	for (size_t i = 0; i < foundNodeCount; i++) {
		for (size_t j = 0; i < foundNodeCount && j < 20; j++, i++) {
			printf("%zu, ", foundNodes[i].travelCost);
		}
		printf("\n");
	}

	// Output.
	if (foundNodeCount > 0) printf("Output: %zu\n", foundNodes[0].travelCost);
	return 0;
}

const size_t getDistance(const size_t x1, const size_t y1, const size_t x2, const size_t y2) {
	const size_t cx = x1 > x2 ? x1 - x2 : x2 - x1, cy = y1 > y2 ? y1 - y2 : y2 - y1;
	return cx + cy;
}