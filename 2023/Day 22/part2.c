// Load FileBuffer.
#define FILEBUFFER_READBUFFERSIZE 100
enum ReadState {
	ReadState_Line,
};
#include "../Common/FileBuffer.h"

#define BRICK_MAX 2000

typedef struct vec3 {
	size_t x, y, z;
} vec3;

typedef struct Brick {
	vec3 pos, size;
	bool fallen;
} Brick;

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
	Brick bricks[BRICK_MAX];
	size_t brickCount = 0;

	// Interpret file.
	while (FileBuffer_ReadUntilNextDelim(&fileBuffer)) {
		// Processing.
		switch (fileBuffer.readState) {
			case ReadState_Line: {
				// Read values.
				size_t values[6], valueCount = 0;
				char* valueStr = strtok(fileBuffer.readBuffer, ",~");
				while (valueStr != nullptr) {
					// Read data.
					sscanf(valueStr, "%zu", &values[valueCount++]);

					// Increment.
					valueStr = strtok(nullptr, ",~");
				}

				// Error check.
				if (brickCount >= BRICK_MAX) {
					fprintf(stderr, "Too many bricks!\n");
					abort();
				}

				// Handle values.
				bricks[brickCount++] = (Brick) {
					.pos = {
						.x = values[0],
						.y = values[1],
						.z = values[2]
					},
					.size = {
						.x = (values[3] + 1) - values[0],
						.y = (values[4] + 1) - values[1],
						.z = (values[5] + 1) - values[2]
					},
					.fallen = false,
				};
			} break;
		}
	}

	// NOTE: Cant wait to optimize this in part 2 lol.

	{
		// Settle bricks.
		bool settled = false;
		while (!settled) {
			settled = true;

			// Move bricks.
			for (size_t i = 0; i < brickCount; i++) {
				// Get base brick.
				Brick* const base = &bricks[i];
				const vec3 baseMax = { base->pos.x + base->size.x, base->pos.y + base->size.y, base->pos.z + base->size.z };

				bool canMove = base->pos.z > 1;
				for (size_t j = 0; canMove && j < brickCount; j++) {
					// Only check other bricks.
					if (j == i) continue;

					// Get target brick.
					Brick* target = &bricks[j];
					const vec3 targetMax = { target->pos.x + target->size.x, target->pos.y + target->size.y, target->pos.z + target->size.z };

					// Check if bounds overlapping.
					const bool
						xOverlap = base->pos.x < targetMax.x && baseMax.x > target->pos.x,
						yOverlap = base->pos.y < targetMax.y && baseMax.y > target->pos.y,
						zOverlap = base->pos.z < targetMax.z + 1 && baseMax.z > target->pos.z;

					// Check if overlapping.
					if (xOverlap && yOverlap && zOverlap)
						canMove = false;
				}

				if (canMove) {
					// Move!
					base->pos.z--;
					settled = false;
				}
			}
		}
	}

	Brick tempBricks[BRICK_MAX];
	size_t totalFallCount = 0;
	for (size_t n = 0; n < brickCount; n++) {
		memcpy(tempBricks, bricks, sizeof(Brick) * brickCount);

		bool settled = false;
		while (!settled) {
			settled = true;

			// Move temp bricks.
			for (size_t i = 0; i < brickCount; i++) {
				if (i == n) continue;

				// Get base brick.
				Brick* const base = &tempBricks[i];
				const vec3 baseMax = { base->pos.x + base->size.x, base->pos.y + base->size.y, base->pos.z + base->size.z };

				bool canMove = base->pos.z > 1;
				for (size_t j = 0; canMove && j < brickCount; j++) {
					// Only check other bricks.
					if (j == i || j == n) continue;

					// Get target brick.
					Brick* target = &tempBricks[j];
					const vec3 targetMax = { target->pos.x + target->size.x, target->pos.y + target->size.y, target->pos.z + target->size.z };

					// Check if bounds overlapping.
					const bool
						xOverlap = base->pos.x < targetMax.x && baseMax.x > target->pos.x,
						yOverlap = base->pos.y < targetMax.y && baseMax.y > target->pos.y,
						zOverlap = base->pos.z < targetMax.z + 1 && baseMax.z > target->pos.z;

					// Check if overlapping.
					if (xOverlap && yOverlap && zOverlap)
						canMove = false;
				}

				if (canMove) {
					// Move!
					base->pos.z--;
					settled = false;
					base->fallen = true;
				}
			}
		}

		for (size_t i = 0; i < brickCount; i++)
			if (tempBricks[i].fallen)
				totalFallCount++;
	}

	// Output answer.
	printf("Output: %zu\n", totalFallCount);
	return 0;
}