// Load FileBuffer.
#define FILEBUFFER_READBUFFERSIZE 1000
enum ReadState {
	ReadState_Line,
};
#include "../Common/FileBuffer.h"

typedef long long int SequenceType;

const SequenceType getNextSequenceIncrement(
	SequenceType* const sequenceStorage, size_t* const sequenceStorageCount,
	const size_t lookStart, const size_t lookCount
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
	SequenceType total = 0;
	SequenceType* sequenceStorage = malloc(50000 * sizeof(SequenceType));
	size_t sequenceStorageCount = 0;

	// Interpret file.
	while (FileBuffer_ReadUntilNextDelim(&fileBuffer)) {
		// Processing.
		switch (fileBuffer.readState) {
			case ReadState_Line: {
				// Reset storage.
				sequenceStorageCount = 0;

				// Read values.
				char* valueStr = strtok(fileBuffer.readBuffer, " ");
				while (valueStr != nullptr) {
					// Read data.
					sscanf(valueStr, "%lli", &sequenceStorage[sequenceStorageCount]);
					sequenceStorageCount++;

					// Increment.
					valueStr = strtok(nullptr, " ");
				}
				// Remember last index.
				const size_t lastIndex = sequenceStorageCount - 1;

				// Calculate change.
				const SequenceType newChange = getNextSequenceIncrement(sequenceStorage, &sequenceStorageCount, 0, sequenceStorageCount);

				// Add to total.
				total += sequenceStorage[lastIndex] + newChange;
			} break;
		}
	}

	// Output answer.
	printf("Output: %lli\n", total);
	return 0;
}

const SequenceType getNextSequenceIncrement(
	SequenceType* const sequenceStorage, size_t* const sequenceStorageCount,
	const size_t lookStart, const size_t lookCount
) {
	// Get start index.
	const size_t startIndex = *sequenceStorageCount;

	bool allZeros = true;
	for (size_t i = 1; i < lookCount; i++) {
		// Get difference.
		const SequenceType change =
			sequenceStorage[i + lookStart] -
			sequenceStorage[i + lookStart - 1];

		// Store.
		sequenceStorage[*sequenceStorageCount] = change;

		// Check for all zeros.
		if (change != 0)
			allZeros = false;

		// Increment.
		(*sequenceStorageCount)++;
	}

	// End case.
	if (allZeros)
		return 0;

	// Get end index.
	const size_t endIndex = *sequenceStorageCount;

	// Get the relative change.
	const SequenceType relativeChange = getNextSequenceIncrement(
		sequenceStorage, sequenceStorageCount,
		startIndex, endIndex - startIndex
	);

	// Return the relative change.
	return sequenceStorage[endIndex - 1] + relativeChange;
}
