// Load FileBuffer.
#define FILEBUFFER_READBUFFERSIZE 100
enum ReadState {
	ReadState_Heading,
	ReadState_Line,
};
#include "../Common/FileBuffer.h"

enum TargetValue {
	TargetValue_Time,
	TargetValue_Distance,
};

struct RaceInformation {
	size_t m_totalTime, m_totalDistance;
};

// Entry point.
int main() {
	// Create file buffer.
	struct FileBuffer fileBuffer;
	FileBuffer_OpenFile(&fileBuffer, "input.txt");

	// File delims.
	struct FileDelim delims[] = {
		{ ':', ReadState_Heading },
		{ '\n', ReadState_Line }
	};
	FileBuffer_SetDelims(&fileBuffer, delims, sizeof(delims) / sizeof(*delims), ReadState_Line);

	// Interpretation variables.
	enum TargetValue targetValue;
	size_t raceInformationCount = 0;
	struct RaceInformation raceInformation[20];

	// Interpret file.
	while (FileBuffer_ReadUntilNextDelim(&fileBuffer)) {
		// Processing.
		switch (fileBuffer.readState) {
			case ReadState_Heading: {
				if (strcmp(fileBuffer.readBuffer, "Time") == 0) targetValue = TargetValue_Time;
				else if (strcmp(fileBuffer.readBuffer, "Distance") == 0) targetValue = TargetValue_Distance;
			} break;
			case ReadState_Line: {
				size_t valueCount = 0;
				char* valueStr = strtok(fileBuffer.readBuffer, " ");
				while (valueStr != nullptr) {
					// Read value.
					switch (targetValue) {
						case TargetValue_Time: sscanf(valueStr, "%zu", &raceInformation[valueCount++].m_totalTime); break;
						case TargetValue_Distance: sscanf(valueStr, "%zu", &raceInformation[valueCount++].m_totalDistance); break;
					}

					// Increment.
					valueStr = strtok(nullptr, " ");
				}

				// Update details.
				if (raceInformationCount < valueCount)
					raceInformationCount = valueCount;
			} break;
		}
	}

	// Process
	size_t totalNumberOfWaysToWin = 0;
	for (size_t i = 0; i < raceInformationCount; i++) {
		// Get race.
		struct RaceInformation* race = &raceInformation[i];
		size_t raceTotalWays = 0;

		// Brute force.
		for (size_t j = 1; j < race->m_totalTime; j++) {
			const size_t
				heldTime = j,
				raceTime = race->m_totalTime - heldTime,
				raceDistance = raceTime * heldTime;

			if (raceDistance > race->m_totalDistance)
				raceTotalWays++;
		}

		if (totalNumberOfWaysToWin == 0) totalNumberOfWaysToWin = raceTotalWays;
		else totalNumberOfWaysToWin *= raceTotalWays;
	}

	// Output answer.
	printf("Output: %zu\n", totalNumberOfWaysToWin);
	return 0;
}