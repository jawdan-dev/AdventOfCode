// Load FileBuffer.
#define FILEBUFFER_READBUFFERSIZE 300
enum ReadState {
	ReadState_Line,
};
#include "../Common/FileBuffer.h"

enum MirrorType {
	MirrorType_Ground,
	MirrorType_SplitterLR,
	MirrorType_SplitterTB,
	MirrorType_MirrorBLTR,
	MirrorType_MirrorTLBR,
};
enum CurrentDirection {
	CurrentDirection_North = 0b1,
	CurrentDirection_East = 0b10,
	CurrentDirection_South = 0b100,
	CurrentDirection_West = 0b1000,
};

void traverseLight(
	const enum MirrorType* const world, int* const traverseState,
	const size_t worldWidth, const size_t worldHeight,
	const size_t beamX, const size_t beamY, const enum CurrentDirection beamDirection
);
const size_t getTotalTraversed(
	const enum MirrorType* const world, const size_t worldWidth, const size_t worldHeight,
	const size_t beamX, const size_t beamY, const enum CurrentDirection beamDirection
);

#define WORLD_MAXWIDTH FILEBUFFER_READBUFFERSIZE
#define WORLD_MAXHEIGHT WORLD_MAXWIDTH

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
	enum MirrorType world[WORLD_MAXWIDTH * WORLD_MAXHEIGHT];
	size_t width = 0, height = 0;;

	// Interpret file.
	while (FileBuffer_ReadUntilNextDelim(&fileBuffer)) {
		// Processing.
		switch (fileBuffer.readState) {
			case ReadState_Line: {
				const size_t bufferSize = strlen(fileBuffer.readBuffer);

				// Update with.
				if (width < bufferSize)
					width = bufferSize;

				// Read buffer.
				for (size_t i = 0; i < bufferSize; i++) {
					enum MirrorType type = MirrorType_Ground;
					switch (fileBuffer.readBuffer[i]) {
						case '|': type = MirrorType_SplitterTB; break;
						case '-': type = MirrorType_SplitterLR; break;
						case '/': type = MirrorType_MirrorBLTR; break;
						case '\\': type = MirrorType_MirrorTLBR; break;
					}
					// Update world tile.
					world[i + (height * WORLD_MAXWIDTH)] = type;
				}

				// Update height;
				height++;
			} break;
		}
	}

	// Get total traversed from top left.
	const size_t totalTraversal =
		getTotalTraversed(world, width, height, 0, 0, CurrentDirection_East);

	// Output answer.
	printf("Output: %zu\n", totalTraversal);
	return 0;
}


void traverseLight(
	const enum MirrorType* const world, int* const traverseState,
	const size_t worldWidth, const size_t worldHeight,
	const size_t beamX, const size_t beamY, const enum CurrentDirection beamDirection
) {
	// Create non-const variables for traversal use.
	size_t x = beamX, y = beamY;

	while (x < worldWidth && y < worldHeight) {
		if (traverseState[x + (y * WORLD_MAXWIDTH)] & beamDirection)
			// Ignore if on already traversed path.
			return;

		// Update traversal state.
		traverseState[x + (y * WORLD_MAXWIDTH)] |= beamDirection;

		switch (world[x + (y * WORLD_MAXWIDTH)]) {
			// Handle diagonal mirrors:
			// NOTE: Must be offset in new traversal direction as it would spin on the mirror.
			case MirrorType_MirrorBLTR: {
				switch (beamDirection) {
					case CurrentDirection_North: traverseLight(world, traverseState, worldWidth, worldHeight, x + 1, y, CurrentDirection_East); break;
					case CurrentDirection_East: traverseLight(world, traverseState, worldWidth, worldHeight, x, y - 1, CurrentDirection_North); break;
					case CurrentDirection_South: traverseLight(world, traverseState, worldWidth, worldHeight, x - 1, y, CurrentDirection_West); break;
					case CurrentDirection_West: traverseLight(world, traverseState, worldWidth, worldHeight, x, y + 1, CurrentDirection_South); break;
				}
			} return;
			case MirrorType_MirrorTLBR: {
				switch (beamDirection) {
					case CurrentDirection_North: traverseLight(world, traverseState, worldWidth, worldHeight, x - 1, y, CurrentDirection_West); break;
					case CurrentDirection_East: traverseLight(world, traverseState, worldWidth, worldHeight, x, y + 1, CurrentDirection_South); break;
					case CurrentDirection_South: traverseLight(world, traverseState, worldWidth, worldHeight, x + 1, y, CurrentDirection_East); break;
					case CurrentDirection_West: traverseLight(world, traverseState, worldWidth, worldHeight, x, y - 1, CurrentDirection_North); break;
				}
			} return;

			// Handle splitters.
			case MirrorType_SplitterLR: {
				// Ignore parrallel movement.
				if (beamDirection == CurrentDirection_East || beamDirection == CurrentDirection_West) break;
				// Traverse split routes.
				traverseLight(world, traverseState, worldWidth, worldHeight, x, y, CurrentDirection_East);
				traverseLight(world, traverseState, worldWidth, worldHeight, x, y, CurrentDirection_West);
			} return;
			case MirrorType_SplitterTB: {
				// Ignore parrallel movement.
				if (beamDirection == CurrentDirection_North || beamDirection == CurrentDirection_South) break;
				// Traverse split routes.
				traverseLight(world, traverseState, worldWidth, worldHeight, x, y, CurrentDirection_North);
				traverseLight(world, traverseState, worldWidth, worldHeight, x, y, CurrentDirection_South);
			} return;
		}

		// Update position.
		switch (beamDirection) {
			case CurrentDirection_North: y--; break;
			case CurrentDirection_East: x++; break;
			case CurrentDirection_South: y++; break;
			case CurrentDirection_West: x--; break;
		}
	}
}
const size_t getTotalTraversed(
	const enum MirrorType* const world, const size_t worldWidth, const size_t worldHeight,
	const size_t beamX, const size_t beamY, const enum CurrentDirection beamDirection
) {
	// Create world state buffer.
	int traverseState[WORLD_MAXWIDTH * WORLD_MAXHEIGHT];
	for (size_t i = 0; i < worldWidth; i++) {
		for (size_t j = 0; j < worldHeight; j++) {
			traverseState[i + (j * WORLD_MAXWIDTH)] = 0;
		}
	}

	// Traverse.
	traverseLight(world, traverseState, worldWidth, worldHeight, beamX, beamY, beamDirection);

	// Count total tiles traversed.
	size_t totalTraversal = 0;
	for (size_t i = 0; i < worldWidth; i++) {
		for (size_t j = 0; j < worldHeight; j++) {
			if (traverseState[i + (j * WORLD_MAXWIDTH)])
				// Inc. count.
				totalTraversal++;
		}
	}

	// Return total traversed.
	return totalTraversal;
}