// Load FileBuffer.
#define FILEBUFFER_READBUFFERSIZE 100
enum ReadState {
	ReadState_Line,
};
#include "../Common/FileBuffer.h"

enum SpringState {
	SpringState_Operational,
	SpringState_Damaged,
	SpringState_Unknown,
};
const bool isSpace(const enum SpringState state);
const bool isDamaged(const enum SpringState state);

const size_t getPossibleArrangementCountRecursive(
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
	enum SpringState currentLine[FILEBUFFER_READBUFFERSIZE];
	size_t currentLineTargets[FILEBUFFER_READBUFFERSIZE];

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
					sscanf(values, "%zu", &currentLineTargets[targetCount]);
					targetCount++;
					// Increment.
					values = strtok(nullptr, ",");
				}

				// Get possible arrangementCount.
				const size_t arrangementCount = getPossibleArrangementCountRecursive(
					currentLine, currentLineSize,
					currentLineTargets, targetCount
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

const size_t getPossibleArrangementCountRecursive(
	enum SpringState* const currentState, const size_t currentStateLength,
	const size_t* const targets, const size_t targetCount
) {
	if (targetCount <= 0) {
		// Check for any trailing springs.
		for (size_t i = 0; i < currentStateLength; i++)
			if (!isSpace(currentState[i]))
				// Trailing spring found.
				return 0;

		// No trailing springs found.
		return 1;
	}

	// Calculate total space needed.
	size_t totalSpaceNeeded = 0;
	for (size_t i = 0; i < targetCount; i++)
		totalSpaceNeeded += targets[i] + (i < targetCount - 1 ? 1 : 0);
	// Skip if not enought space.
	if (totalSpaceNeeded > currentStateLength)
		return 0;

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
			currentState + offset, offset <= currentStateLength ? currentStateLength - offset : 0,
			targets + 1, targetCount - 1
		);
		localTotal += total;
	}

	// Return total.
	return localTotal;
}
