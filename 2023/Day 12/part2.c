// Load FileBuffer.
#define FILEBUFFER_READBUFFERSIZE 100
enum ReadState {
	ReadState_Line,
};
#include "../Common/FileBuffer.h"
struct MemoryKey {
	size_t remainingLength, remainingTargets;
};
typedef size_t MemoryValue;
#include "../Common/Memorize.h"

enum SpringState {
	SpringState_Operational,
	SpringState_Damaged,
	SpringState_Unknown,
};
const bool isSpace(const enum SpringState state);
const bool isDamaged(const enum SpringState state);

const size_t getPossibleArrangementCountRecursive(
	struct Memory* memory,
	enum SpringState* const currentState, const size_t currentStateLength,
	const size_t* const targetLayout, const size_t targetCount
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
	size_t totalArrangementCount = 0;

#	define UNFOLD_COUNT 5
	enum SpringState currentLine[FILEBUFFER_READBUFFERSIZE * UNFOLD_COUNT];
	size_t currentTargets[FILEBUFFER_READBUFFERSIZE * UNFOLD_COUNT];

	struct Memory memory;

	// Interpret file.
	while (FileBuffer_ReadUntilNextDelim(&fileBuffer)) {
		// Processing.
		switch (fileBuffer.readState) {
			case ReadState_Line: {
				// Error check.
				if (strlen(fileBuffer.readBuffer) >= sizeof(currentLine) / sizeof(*currentLine)) {
					fprintf(stderr, "Line buffer size exceeded.\n");
					abort();
				}

				// Interpretation variables.
				size_t damagedCount = 0;
				size_t targetCount = 0;

				// Read line.
				char* values = strtok(fileBuffer.readBuffer, " ");
				const size_t currentLineSize = strlen(values);
				for (size_t i = 0; i < currentLineSize; i++) {
					switch (values[i]) {
						case '.': currentLine[i] = SpringState_Operational; break;
						case '#': currentLine[i] = SpringState_Damaged; damagedCount++; break;
						case '?': currentLine[i] = SpringState_Unknown; break;
					}
				}

				// Read targets.
				values = strtok(nullptr, " ");
				values = strtok(values, ",");
				while (values != nullptr) {
					sscanf(values, "%zu", &currentTargets[targetCount]);
					targetCount++;
					// Increment.
					values = strtok(nullptr, ",");
				}

				// Create unfolded data.
				currentLine[currentLineSize] = SpringState_Unknown;
				for (size_t i = 1; i < UNFOLD_COUNT; i++) {
					// Unfold line.
					const size_t lineOffset = i * (currentLineSize + 1);
					for (size_t j = 0; j <= currentLineSize; j++) {
						currentLine[lineOffset + j] = currentLine[j];
					}

					// Unfold targets.
					const size_t targetOffset = i * targetCount;
					for (size_t j = 0; j < targetCount; j++) {
						currentTargets[targetOffset + j] = currentTargets[j];
					}
				}

				// Reset memory.
				Memory_reset(&memory);

				// Get possible arrangementCount.
				const size_t arrangementCount = getPossibleArrangementCountRecursive(
					&memory,
					currentLine, (currentLineSize * UNFOLD_COUNT) + (UNFOLD_COUNT - 1),
					currentTargets, (targetCount * UNFOLD_COUNT)
				);
				totalArrangementCount += arrangementCount;
			} break;
		}
	}

	// Output answer.
	printf("Output: %zu\n", totalArrangementCount);
	return 0;
}

const bool isSpace(const enum SpringState state) {
	return
		state == SpringState_Operational ||
		state == SpringState_Unknown;
}
const bool isDamaged(const enum SpringState state) {
	return
		state == SpringState_Damaged ||
		state == SpringState_Unknown;
}

// NOTE: New memorization hotness :)
const size_t getPossibleArrangementCountRecursive(
	struct Memory* memory,
	enum SpringState* const currentState, const size_t currentStateLength,
	const size_t* const targets, const size_t targetCount
) {
	// Find if value cached.
	const struct MemoryKey memoryKey = { currentStateLength, targetCount };
	const MemoryValue* const value = Memory_find(memory, memoryKey);
	if (value != nullptr)
		return *value;

	if (targetCount <= 0) {
		// Check for any trailing springs.
		for (size_t i = 0; i < currentStateLength; i++)
			if (!isSpace(currentState[i]))
				// Trailing spring found.
				return *Memory_add(memory, memoryKey, 0);

		// No trailing springs found.
		return *Memory_add(memory, memoryKey, 1);
	}

	// Calculate total space needed.
	size_t totalSpaceNeeded = 0;
	for (size_t i = 0; i < targetCount; i++)
		totalSpaceNeeded += targets[i] + (i < targetCount - 1 ? 1 : 0);
	// Skip if not enought space.
	if (totalSpaceNeeded > currentStateLength)
		return *Memory_add(memory, memoryKey, 0);

	// Storage.
	size_t localTotal = 0;

	// Calculate total possible.
	const size_t possibleOffsetCount = (currentStateLength - totalSpaceNeeded) + 1;
	for (size_t i = 0; i < possibleOffsetCount; i++) {
		// Check for spaces before.
		if (i > 0 && currentStateLength > i && !isSpace(currentState[i - 1]))
			break;

		// Check for damaged.
		bool valid = true;
		for (size_t j = 0; valid && j < targets[0]; j++)
			if (i + j >= currentStateLength || !isDamaged(currentState[i + j]))
				valid = false;

		// Check for space after.
		const size_t spaceAfterIndex = i + targets[0];
		if (valid && currentStateLength > spaceAfterIndex && !isSpace(currentState[spaceAfterIndex]))
			valid = false;


		// Skip if not valid.
		if (!valid) continue;

		// Get lenth.
		const size_t length = targets[0] + 1;
		const size_t offset = i + length;

		// Get total.
		const size_t total = getPossibleArrangementCountRecursive(
			memory,
			currentState + offset, offset <= currentStateLength ? currentStateLength - offset : 0,
			targets + 1, targetCount - 1
		);
		localTotal += total;
	}

	// Return total.
	return *Memory_add(memory, memoryKey, localTotal);
}
