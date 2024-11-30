// Load FileBuffer.
#define FILEBUFFER_READBUFFERSIZE 100
enum ReadState {
	ReadState_Line,
};
#include "../Common/FileBuffer.h"

#define DIRECTION_MOD 4
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
	size_t trueIndex;
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
				// Get hex string.
				strtok(fileBuffer.readBuffer, "#");
				const char* const hexStr = strtok(nullptr, "#");

				// Read data.
				char digDirectionC;
				_snscanf(hexStr + 5, 1, "%c", &digDirectionC);
				size_t digAmount;
				_snscanf(hexStr, 5, "%x", &digAmount);

				// Current instruction.
				if (instructionCount >= INSTRUCTIONS_MAX) {
					fprintf(stderr, "Instruction max exceeded.\n");
					abort();
				}
				struct DigInstruction* instruction = &instructions[instructionCount++];

				// Set instruction values.
				instruction->digAmount = digAmount;
				switch (digDirectionC) {
					case '3': instruction->direction = Direction_North; break;
					case '1': instruction->direction = Direction_South; break;
					case '2': instruction->direction = Direction_West; break;
					case '0': instruction->direction = Direction_East; break;
				}
				instruction->trueIndex = instructionCount - 1;
			} break;

		}
	}

	// NOTE: 'Expanding-Bucket' Algorithm (Trademarked me.) ^tm.

	size_t expansionSize = 0;
	// Get total outside area sum.
	for (size_t i = 0; i < instructionCount; i++)
		expansionSize += instructions[i].digAmount;

	// +1 due to inside/outside corner summed area.
	const size_t outsideArea = (expansionSize / 2) + 1;
	printf("Found the extra perimeter area %zu\n", outsideArea);


	// NOTE: 'Collapsing Bucket' Algorithm (if that name / concept isnt taken already lol) ^tm.

	// Calculate total positve / negative.
	size_t positiveVolume = 0, negativeVolume = 0;
	size_t activeBucketIndex = 0;
	while (instructionCount > 0) {
		// Make sure active bucket is in range.
		activeBucketIndex %= instructionCount;

		// Get indices.
		const size_t
			a = (activeBucketIndex + (instructionCount - 1)) % instructionCount,
			b = activeBucketIndex,
			c = (activeBucketIndex + 1) % instructionCount;

		// Get instructions.
		struct DigInstruction
			* const ai = &instructions[a],
			* const bi = &instructions[b],
			* const ci = &instructions[c];

		// Check if any can be removed.
		if (ai->digAmount > 0 &&
			bi->digAmount > 0 &&
			ci->digAmount > 0) {

			// Invert directions for later use.
			const Direction
				aid = ((ai->direction + 2) % DIRECTION_MOD),
				cid = ((ci->direction + 2) % DIRECTION_MOD);

			// Check for collapse-ables.
			if (ai->direction == bi->direction) {
				// Collapse positively.
				bi->digAmount += ai->digAmount;
				ai->digAmount = 0;
			} else if (aid == bi->direction) {
				// Collapse negatively.
				const size_t amount = __min(ai->digAmount, bi->digAmount);
				bi->digAmount -= amount;
				ai->digAmount -= amount;
			} else if (ci->direction == bi->direction) {
				// Collapse positively.
				bi->digAmount += ci->digAmount;
				ci->digAmount = 0;
			} else if (cid == bi->direction) {
				// Collapse negatively.
				const size_t amount = __min(ci->digAmount, bi->digAmount);
				bi->digAmount -= amount;
				ci->digAmount -= amount;
			} else {
				if (a == b && (ai->digAmount > 0) ||
					b == c && (bi->digAmount > 0) ||
					c == a && (ci->digAmount > 0)) {
					// Error found.
					fprintf(stderr, "Error: Matching indices occured in find with non-zero lengths.\n");
					printf("a: %zu with length %zu.\n", a, ai->digAmount);
					printf("b: %zu with length %zu.\n", b, bi->digAmount);
					printf("c: %zu with length %zu.\n", c, ci->digAmount);
					abort();
				} else if (aid == ci->direction) {
					// Collapse bucket.
					const size_t bucketLength = __min(ai->digAmount, ci->digAmount);
					ai->digAmount -= bucketLength;
					ci->digAmount -= bucketLength;

					// Get area.
					const size_t area = bucketLength * bi->digAmount;

					// Add to volume.
					if ((ai->direction + 1) % DIRECTION_MOD == bi->direction) positiveVolume += area;
					else negativeVolume += area;
					printf("Found bucket {%zu x %zu} = (%zu).\n", bucketLength, bi->digAmount, area);
				} else {
					// Move to the next bucket to try.
					activeBucketIndex = (activeBucketIndex + 1) % instructionCount;
				}
			}
		}

		// Check if instructions can be removed (one at a time).
		size_t removeStart = SIZE_MAX;
		if (ai->digAmount == 0) removeStart = a;
		else if (bi->digAmount == 0) removeStart = b;
		else if (ci->digAmount == 0) removeStart = c;

		// Check if any are to be removed.
		if (removeStart == SIZE_MAX) continue;

		// Remove instruction.
		for (size_t i = removeStart + 1; i < instructionCount; i++) {
			// Move others.
			instructions[i - 1] = instructions[i];
			// Make sure active bucket is correct.
			if (i == activeBucketIndex) activeBucketIndex = i - 1;
		}
		// Update details.
		instructionCount--;
	}

	// Calculate total volume.
	const size_t totalVolume =
		outsideArea +
		__max(positiveVolume, negativeVolume) -
		__min(positiveVolume, negativeVolume);

	// Output answer.
	printf("Output: %zu\n", totalVolume);
	return 0;
}