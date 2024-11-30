// Load FileBuffer.
#define FILEBUFFER_READBUFFERSIZE 200
enum ReadState {
	ReadState_Line,
};
#include "../Common/FileBuffer.h"

enum Direction {
	Direction_None,
	Direction_North,
	Direction_East,
	Direction_South,
	Direction_West,
};
const enum Direction Direction_flip(const enum Direction direction);

enum PipeType {
	PipeType_Vertical,
	PipeType_Horizontal,
	PipeType_90ne,
	PipeType_90nw,
	PipeType_90se,
	PipeType_90sw,
	PipeType_Ground,
	PipeType_StartingPosition,
};
const enum Direction Pipe_nextDirection(const enum PipeType pipe, const enum Direction entryDirection);

// Interpretation table.
const struct {
	char m_character;
	enum PipeType m_pipeType;
} s_PipeInterpretationTable[] = {
	{ '|', PipeType_Vertical },
	{ '-', PipeType_Horizontal },
	{ 'L', PipeType_90ne },
	{ 'J', PipeType_90nw },
	{ 'F', PipeType_90se },
	{ '7', PipeType_90sw },
	{ '.', PipeType_Ground },
	{ 'S', PipeType_StartingPosition },
};

#define Map_maxWidth FILEBUFFER_READBUFFERSIZE
#define Map_maxHeight 200
struct Map {
	enum PipeType m_layout[Map_maxWidth][Map_maxHeight];
	size_t m_width, m_height;
};
// Recursive maze back tracking.
const size_t Map_recurseTraverse(
	struct Map* const this,
	const size_t x, const size_t y,
	enum Direction entryDirection
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
	struct Map map;
	map.m_width = 0;
	map.m_height = 0;
	//
	bool startFound = false;
	size_t startingX = 0, startingY = 0;
	//

	// Interpret file.
	while (FileBuffer_ReadUntilNextDelim(&fileBuffer)) {
		// Processing.
		switch (fileBuffer.readState) {
			case ReadState_Line: {
				size_t totalWidth = strlen(fileBuffer.readBuffer);
				if (map.m_width < totalWidth)
					map.m_width = totalWidth;

				// Size limits check.
				if (map.m_width > Map_maxWidth || map.m_height > Map_maxHeight) {
					fprintf(stderr, "Map size limits of [%zu, %zu] exceeded with [%zu, %zu]\n", Map_maxWidth, Map_maxHeight, map.m_width, map.m_height);
					abort();
				}


				// Interpret values.
				for (size_t i = 0; i < totalWidth; i++) {
					for (size_t j = 0; j < sizeof(s_PipeInterpretationTable) / sizeof(*s_PipeInterpretationTable); j++) {
						if (fileBuffer.readBuffer[i] != s_PipeInterpretationTable[j].m_character) continue;

						if (s_PipeInterpretationTable[j].m_pipeType == PipeType_StartingPosition) {
							startFound = true;
							startingX = i;
							startingY = map.m_height;
						}

						// Update table variable.
						map.m_layout[i][map.m_height] = s_PipeInterpretationTable[j].m_pipeType;
						break;
					}
				}

				map.m_height++;
			} break;
		}
	}

	if (startFound) {
		// Create checks list.
		const struct {
			size_t x, y;
			enum Direction direction;
		} checks[] = {
			{ startingX, startingY - 1, Direction_North },
			{ startingX + 1, startingY, Direction_East },
			{ startingX, startingY + 1, Direction_South },
			{ startingX - 1, startingY, Direction_West },
		};

		size_t bestScore = 0;
		for (size_t i = 0; i < sizeof(checks) / sizeof(*checks); i++) {
			// Get length.
			const size_t length = Map_recurseTraverse(&map, checks[i].x, checks[i].y, checks[i].direction);
			if (length == 0) continue;

			// Get score.
			const size_t score = (length + 1) / 2;
			// Compare score.
			if (bestScore < score)
				bestScore = score;
		}

		// Output answer.
		printf("Output: %zu\n", bestScore);
	} else {
		fprintf(stderr, "Start not found.\n");
	}

	return 0;
}


const enum Direction Direction_flip(const enum Direction direction) {
	switch (direction) {
		case Direction_North: return Direction_South;
		case Direction_East: return Direction_West;
		case Direction_South: return Direction_North;
		case Direction_West: return Direction_East;
	}
	return Direction_None;
}

const enum Direction Pipe_nextDirection(const enum PipeType pipe, const enum Direction entryDirection) {
	const enum Direction fDirection = Direction_flip(entryDirection);
	switch (pipe) {
#		define Pipe_InOut(dir1, dir2) \
			if (fDirection == dir1) { return dir2; } \
			else if (fDirection == dir2) { return dir1; } \
			break

		case PipeType_Vertical: Pipe_InOut(Direction_North, Direction_South);
		case PipeType_Horizontal: Pipe_InOut(Direction_East, Direction_West);
		case PipeType_90ne: Pipe_InOut(Direction_North, Direction_East);
		case PipeType_90nw: Pipe_InOut(Direction_North, Direction_West);
		case PipeType_90se: Pipe_InOut(Direction_South, Direction_East);
		case PipeType_90sw: Pipe_InOut(Direction_South, Direction_West);

#		undef Pipe_InOut
	}
	return Direction_None;
}

const size_t Map_recurseTraverse(
	struct Map* const this,
	const size_t x, const size_t y,
	enum Direction entryDirection
) {
	// Base cases.
	if (x >= this->m_width || y >= this->m_height) return 0;
	if (this->m_layout[x][y] == PipeType_StartingPosition) {
		return 1;
	}

	// Get next direction.
	enum Direction nextDirection = Pipe_nextDirection(this->m_layout[x][y], entryDirection);
	// Find nexy x & y.
	size_t nextX = x, nextY = y;
	switch (nextDirection) {
		default: return 0;
		case Direction_North: nextY--; break;
		case Direction_East: nextX++; break;
		case Direction_South: nextY++; break;
		case Direction_West: nextX--; break;
	}

	// Get length of next path.
	const size_t length = Map_recurseTraverse(this, nextX, nextY, nextDirection);
	if (length == 0) return 0;

	// Return incremented length.
	return length + 1;
}
