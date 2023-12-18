// Load FileBuffer.
#define FILEBUFFER_READBUFFERSIZE 100
enum ReadState {
	ReadState_Line,
};
#include "../Common/FileBuffer.h"

typedef enum Direction Direction;
enum Direction {
	Direction_North,
	Direction_East,
	Direction_South,
	Direction_West,
};

struct DigInstruction {
	Direction direction;
	size_t digAmount;
};

struct Tile {
	bool dug;
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
#	define INSTRUCTIONS_MAX 1000
	struct DigInstruction instructions[INSTRUCTIONS_MAX];
	size_t instructionCount = 0;

	// Interpret file.
	while (FileBuffer_ReadUntilNextDelim(&fileBuffer)) {
		// Processing.
		switch (fileBuffer.readState) {
			case ReadState_Line: {
				// Read data.
				char digDirectionC;
				size_t digAmount;
				sscanf(fileBuffer.readBuffer, "%c %zu", &digDirectionC, &digAmount);

				// Current instruction.
				if (instructionCount >= INSTRUCTIONS_MAX) {
					fprintf(stderr, "Instruction max exceeded.\n");
					abort();
				}
				struct DigInstruction* instruction = &instructions[instructionCount++];

				// Set instruction values.
				instruction->digAmount = digAmount;
				switch (digDirectionC) {
					case 'U': instruction->direction = Direction_North; break;
					case 'D': instruction->direction = Direction_South; break;
					case 'L': instruction->direction = Direction_West; break;
					case 'R': instruction->direction = Direction_East; break;
				}
			} break;
		}
	}

	// Find max bounds.
	int64_t safeX = 0, safeY = 0;
	int64_t offsetX = safeX, offsetY = safeY,
		maxX = safeX, maxY = safeY;
	for (size_t i = 0; i < instructionCount; i++) {
		const struct DigInstruction* const instruction = &instructions[i];

		switch (instruction->direction) {
			case Direction_North: safeY -= instruction->digAmount; break;
			case Direction_East: safeX += instruction->digAmount; break;
			case Direction_South: safeY += instruction->digAmount; break;
			case Direction_West: safeX -= instruction->digAmount; break;
		}

		if (offsetX < -safeX) offsetX = -safeX;
		if (offsetY < -safeY) offsetY = -safeY;
		if (maxX < safeX) maxX = safeX;
		if (maxY < safeY) maxY = safeY;
	}

	// Get bounds.
	const size_t
		totalWidth = maxX + offsetX + 1,
		totalHeight = maxY + offsetY + 1,
		totalSize = totalWidth * totalHeight;
	// Allocate map.
	struct Tile* tiles = malloc(sizeof(struct Tile) * totalSize);
	if (tiles == nullptr) {
		fprintf(stderr, "Failed to allocate tiles.\n");
		abort();
	}
	for (size_t i = 0; i < totalSize; i++) {
		tiles[i].dug = false;
	}

	// Dig!
	int64_t currentX = 0, currentY = 0;
	for (size_t i = 0; i < instructionCount; i++) {
		const struct DigInstruction* const instruction = &instructions[i];

		for (size_t i = 0; i < instruction->digAmount; i++) {
			// Update position.
			switch (instruction->direction) {
				case Direction_North: currentY--; break;
				case Direction_East: currentX++; break;
				case Direction_South: currentY++; break;
				case Direction_West: currentX--; break;
			}
			// Dig.
			tiles[(currentX + offsetX) + ((currentY + offsetY) * totalWidth)].dug = true;
		}
	}

	// Workout stuff.
	size_t totalVolume = 0;
	for (size_t j = 0; j < totalHeight; j++) {
		const size_t yOffset = j * totalWidth;
		for (size_t i = 0; i < totalWidth; i++) {
			// Ignore if dug.
			if (tiles[i + yOffset].dug) {
				totalVolume++;
				continue;
			}

			// NOTE: Polygon Method.
			size_t polarity = 0;
			for (size_t k = i; k < totalWidth; k++) {
				if (tiles[k + yOffset].dug) {
					// Get if entered through top.
					const size_t entryIndex = k + yOffset - totalWidth;
					const bool topEntry = entryIndex < totalSize && tiles[entryIndex].dug;

					// Iterate to end.
					while (k + 1 < totalWidth && tiles[k + 1 + yOffset].dug) k++;

					// Get if exited through top.
					const size_t exitIndex = k + yOffset + totalWidth;
					const bool bottomExit = exitIndex < totalSize && tiles[exitIndex].dug;

					if (topEntry == bottomExit)
						polarity++;
				}
			}

			// Update volume.
			if (polarity % 2 == 1) {
				totalVolume++;
			}
		}
	}

	// Output answer.
	printf("Output: %zu\n", totalVolume);
	return 0;
}