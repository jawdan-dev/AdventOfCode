// Load FileBuffer.
#define FILEBUFFER_READBUFFERSIZE 150
enum ReadState {
	ReadState_Line,
};
#include "../Common/FileBuffer.h"

#define SEARCHNODES_MAX 2000

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
	Direction direction;
	size_t cost;
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

	// Debug output.
	printf("Tile width x height : %zu x %zu\n", width, height);

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

		PathLocation* path;
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
	searchNodes[0].path = malloc(1 * sizeof(PathLocation));
	searchNodes[0].path[searchNodes[0].pathLength++] = (PathLocation) { .x = searchNodes[0].x, .y = searchNodes[0].y, .direction = searchNodes[0].direction, .cost = searchNodes[0].travelCost };

	// Ignore step.
	typedef struct Cost {
		Direction directions[4];
		size_t costs[4];
		size_t count;
	} Cost;
	Cost bestCost[FILEBUFFER_READBUFFERSIZE][FILEBUFFER_READBUFFERSIZE];
	for (size_t i = 0; i < FILEBUFFER_READBUFFERSIZE; i++) {
		for (size_t j = 0; j < FILEBUFFER_READBUFFERSIZE; j++) {
			bestCost[i][j].count = 0;
		}
	}

	// Found.
	PathLocation* bestFoundPath = nullptr;
	size_t bestFoundPathLength = 0;

	// Search.
	// NOTE: A* time.
	size_t bestPathLength = 0;
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
				// Update best path.
				if (activeNode->travelCost > bestPathLength) {
					bestPathLength = activeNode->travelCost;

					// Update path.
					if (bestFoundPath != nullptr) free(bestFoundPath);
					bestFoundPath = malloc(activeNode->pathLength * sizeof(PathLocation));
					memcpy(bestFoundPath, activeNode->path, activeNode->pathLength * sizeof(PathLocation));
					bestFoundPathLength = activeNode->pathLength;
				}

				// Set path as best.
				for (size_t i = 0; i < activeNode->pathLength; i++) {
					const PathLocation* const location = &activeNode->path[i];

					// Get target cost.
					Cost* const targetCost = &bestCost[location->x][location->y];

					// Find matching cost.
					bool costFound = false;
					for (size_t j = 0; !costFound && j < targetCost->count; j++) {
						if (targetCost->directions[j] != location->direction) continue;
						costFound = true;

						// Update details.
						if (targetCost->costs[j] < location->cost)
							targetCost->costs[j] = location->cost;
					}
					if (!costFound) {
						targetCost->directions[targetCost->count] = location->direction;
						targetCost->costs[targetCost->count] = location->cost;
						targetCost->count++;
					}
				}
			} else {
				// Explore.
				const Direction checks[] = {
					Direction_North, Direction_East,
					Direction_South, Direction_West,
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

					// Make sure we haven't stepped here before.
					bool herePreviously = false;
					for (size_t i = 0; !herePreviously && i < activeNode->pathLength; i++)
						if (activeNode->path[i].x == x && activeNode->path[i].y == y)
							herePreviously = true;
					if (herePreviously) continue;

					// Ignore if stepped already with higher travel cost.
					const size_t travelCost = activeNode->travelCost + 1;

					// Check if tile has a better cost associated with it.
					Cost* const targetCost = &bestCost[x][y];
					bool betterFound = false;
					for (size_t i = 0; i < targetCost->count; i++) {
						if (targetCost->directions[i] != direction) continue;

						// Update details.
						if (targetCost->costs[i] >= travelCost)
							betterFound = true;
						break;
					}
					//if (betterFound) continue;

					// Error checks.
					if (searchNodeCount >= SEARCHNODES_MAX) {
						fprintf(stderr, "Search node max exceeded.\n");
						abort();
					}

					// Place new search node.
					searchNodes[searchNodeCount] = (SearchNode) {
						.x = x, .y = y,
						.direction = direction,
						.distance = getDistance(x, y, targetX, targetY), .travelCost = travelCost,
						.path = nullptr,
						.pathLength = activeNode->pathLength + 1,
					};
					// Copy path.
					searchNodes[searchNodeCount].path = malloc(searchNodes[searchNodeCount].pathLength * sizeof(PathLocation));
					memcpy(searchNodes[searchNodeCount].path, activeNode->path, activeNode->pathLength * sizeof(PathLocation));
					searchNodes[searchNodeCount].path[activeNode->pathLength] = (PathLocation) { .x = x, .y = y, .direction = direction, .cost = travelCost };
					// Update count.
					searchNodeCount++;
				}
			}
		}

		// Cleanup.
		if (activeNode->path != nullptr)
			free(activeNode->path);
		activeNode->path = nullptr;

		// Remove from list.
		if (searchNodeCount > 1) searchNodes[activeI] = searchNodes[searchNodeCount - 1];
		searchNodeCount--;

		// Debug output.
		static size_t printOffset = 0;
		if (printOffset++ > 10000) {
			printf("%zu / %zu | Current best: %zu\t\t\r", searchNodeCount, SEARCHNODES_MAX, bestPathLength);
			printOffset = 0;
		}
	}
	printf("\n");

	for (size_t j = 0; j < height; j++) {
		for (size_t i = 0; i < width; i++) {
			bool found = false;
			for (size_t k = 0; !found && k < bestFoundPathLength; k++)
				if (bestFoundPath[k].x == i && bestFoundPath[k].y == j)
					found = true;

			if (found) printf("#");
			else switch (tiles[i][j]) {
				case PathTile_Path: printf("."); break;
				case PathTile_Forest: printf(" "); break;
				case PathTile_Slope_East: printf(">"); break;
				case PathTile_Slope_South: printf("V"); break;
			}
		}
		printf("\n");
	}

	// Output.
	printf("Output: %zu\n", bestPathLength);
	return 0;
}

const size_t getDistance(const size_t x1, const size_t y1, const size_t x2, const size_t y2) {
	const size_t cx = x1 > x2 ? x1 - x2 : x2 - x1, cy = y1 > y2 ? y1 - y2 : y2 - y1;
	return cx + cy;
}