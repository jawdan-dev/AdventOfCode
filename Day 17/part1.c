// Load FileBuffer.
#define FILEBUFFER_READBUFFERSIZE 200
enum ReadState {
	ReadState_Line,
};
#include "../Common/FileBuffer.h"

enum CurrentDirection {
	CurrentDirection_North,
	CurrentDirection_East,
	CurrentDirection_South,
	CurrentDirection_West,
};

const size_t getDistance(
	const size_t x, const size_t y,
	const size_t tx, const size_t ty
);

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
	uint8_t heatLossMap[FILEBUFFER_READBUFFERSIZE][FILEBUFFER_READBUFFERSIZE];
	size_t width = 0, height = 0;

	// Interpret file.
	while (FileBuffer_ReadUntilNextDelim(&fileBuffer)) {
		// Processing.
		switch (fileBuffer.readState) {
			case ReadState_Line: {
				// Read values.
				const size_t bufferSize = strlen(fileBuffer.readBuffer);

				// Update width.
				if (width < bufferSize)
					width = bufferSize;

				for (size_t i = 0; i < bufferSize; i++) {
					// Read in value.
					_snscanf(fileBuffer.readBuffer + i, 1, "%u", &heatLossMap[i][height]);
				}

				// Update height.
				height++;
			} break;
		}
	}

	// Output info.
	printf("Found dimensions: %zu x %zu\n", width, height);

	// Get target positions.
	const size_t tx = width - 1, ty = height - 1;

	struct Node {
		bool checked;
		enum CurrentDirection direction;
		size_t directionCount;
		size_t x, y;
		size_t travelCost, distance;
	};
#	define NODE_MAX 12
	struct Tile {
		struct Node nodes[NODE_MAX];
		size_t nodeCount;
	}*tiles = malloc(width * height * sizeof(struct Tile));

	// Initialize tiles.
	for (size_t i = 0; i < width; i++)
		for (size_t j = 0; j < height; j++)
			tiles[i + (j * width)].nodeCount = 0;

	// Active tile storage.
	const size_t activeMax = width * height;
	struct Tile** activeTiles = malloc(activeMax * sizeof(struct Tile*));
	size_t activeTileCount = 0;

	// Create starting node.
	tiles[0].nodes[0] = (struct Node) {
		.checked = false,
		.direction = CurrentDirection_East,
		.directionCount = 0,
		.x = 0, .y = 0,
		.travelCost = 0,
		.distance = getDistance(0, 0, tx, ty),
	};
	tiles[0].nodeCount = 1;
	activeTiles[0] = &tiles[0];
	activeTileCount++;

	// Search result storage.
	struct Node* foundNode = nullptr;
	size_t debugPrintCooldown = 0;

	// Search.
	while (activeTileCount > 0) {
		struct Tile* activeTile = nullptr;
		struct Node* activeNode = nullptr;
		size_t bestH;
		// Get best node to expand.
		for (size_t i = 0; i < activeTileCount; i++) {
			struct Tile* const tile = activeTiles[i];
			for (size_t j = 0; j < tile->nodeCount; j++) {
				struct Node* const node = &tile->nodes[j];
				if (node->checked) continue;
				// Calculate heuristic.
				const size_t h = node->travelCost + node->distance;
				if (activeNode == nullptr || activeTile == nullptr || bestH > h) {
					activeTile = tile;
					activeNode = node;
					bestH = h;
				}
			}
		}

		// Shouldn't occur.
		if (activeNode == nullptr || activeTile == nullptr) break;

		// Print.
		if (debugPrintCooldown++ > 1000) {
			debugPrintCooldown = 0;

			// Print best distance.
			size_t bestDistance = SIZE_MAX;
			for (size_t i = 0; i < activeTileCount; i++) {
				struct Tile* const tile = activeTiles[i];
				for (size_t j = 0; j < tile->nodeCount; j++) {
					struct Node* const node = &tile->nodes[j];
					if (bestDistance > node->distance)
						bestDistance = node->distance;
				}
			}
			printf("Best distance from target: %zu | Active tiles: %zu\t\t\r", bestDistance, activeTileCount);
		}

		// Make sure the point is valid.
		if (activeNode->x < width && activeNode->y < height) {
			// Check if found.
			if (activeNode->distance <= 0) {
				foundNode = activeNode;
				break;
			}

			// Configure checks.
			const enum CurrentDirection directions[] = {
				CurrentDirection_North,
				CurrentDirection_East,
				CurrentDirection_South,
				CurrentDirection_West,
			};

			// Check directions.
			for (size_t i = 0; i < sizeof(directions) / sizeof(*directions); i++) {
				const enum CurrentDirection checkDirection = directions[i];

				// Check move distance.
				if (activeNode->direction == checkDirection && activeNode->directionCount >= 3) continue;

				// No going backwards.
				switch (checkDirection) {
					case CurrentDirection_North: if (activeNode->direction == CurrentDirection_South) continue; break;
					case CurrentDirection_East: if (activeNode->direction == CurrentDirection_West) continue; break;
					case CurrentDirection_South: if (activeNode->direction == CurrentDirection_North) continue; break;
					case CurrentDirection_West: if (activeNode->direction == CurrentDirection_East) continue; break;
				}

				// Get new node position.
				size_t x = activeNode->x, y = activeNode->y;
				switch (checkDirection) {
					case CurrentDirection_North: y--; break;
					case CurrentDirection_East: x++; break;
					case CurrentDirection_South: y++; break;
					case CurrentDirection_West: x--; break;
				}
				// Check if out of bounds.
				if (x >= width || y >= height) continue;

				// Get check node.
				struct Node checkNode = {
					.checked = false,
					.direction = checkDirection,
					.directionCount = (activeNode->direction == checkDirection ? activeNode->directionCount + 1 : 1),
					.x = x, .y = y,
					.travelCost = activeNode->travelCost + heatLossMap[x][y],
					.distance = getDistance(x, y, tx, ty),
				};
				// Get check tile.
				struct Tile* const checkTile = &tiles[x + (y * width)];

				// Check for overlapping.
				bool overlappingHandled = false;
				for (size_t j = 0; j < checkTile->nodeCount; j++) {
					struct Node* const node = &checkTile->nodes[j];

					// Check if same.
					if (node->direction != checkNode.direction || node->directionCount != checkNode.directionCount)
						continue;

					// Get heuristics.
					const size_t
						thisH = checkNode.travelCost + checkNode.distance,
						otherH = node->travelCost + node->distance;

					// Compare heuristics.
					if (thisH < otherH)
						*node = checkNode;

					// Overlapping has been handled.
					overlappingHandled = true;
				}
				if (!overlappingHandled) {
					// Error check.
					if (checkTile->nodeCount >= NODE_MAX) {
						fprintf(stderr, "NODE_MAX exceeded.\n");
						abort();
					}

					// Add node.
					checkTile->nodes[checkTile->nodeCount++] =
						checkNode;
				}

				// Find if check tile is active.
				bool activeFound = false;
				for (size_t i = 0; !activeFound && i < activeTileCount; i++)
					if (activeTiles[i] == checkTile)
						activeFound = true;

				if (!activeFound) {
					// Error check.
					if (activeTileCount >= activeMax) {
						fprintf(stderr, "Active tile count exceeded.\n");
						abort();
					}

					// Add to active tiles.
					activeTiles[activeTileCount++] = checkTile;
				}
			}
		}

		// Update details.
		activeNode->checked = true;
		size_t activeNodeCount = 0;
		for (size_t i = 0; i < activeTile->nodeCount; i++)
			if (!activeTile->nodes[i].checked)
				activeNodeCount++;

		// Remove from active list.
		if (activeNodeCount <= 0) {
			for (size_t i = 0; i < activeTileCount; i++) {
				if (activeTiles[i] == activeTile) {
					activeTiles[i] = activeTiles[activeTileCount - 1];
					activeTileCount--;
					break;
				}
			}
		}
	}
	printf("\n");

	if (foundNode != nullptr)
		// Output answer.
		printf("Output: %zu\n", foundNode->travelCost);
	else
		// Output failure.
		printf("Failed to find path.\n");

	// Cleanup.
	free(tiles);
	free(activeTiles);

	return 0;
}

const size_t getDistance(
	const size_t x, const size_t y,
	const size_t tx, const size_t ty
) {
	const size_t
		cx = x > tx ? x - tx : tx - x,
		cy = y > ty ? y - ty : ty - y;
	return cx + cy;
}