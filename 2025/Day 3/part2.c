#include <common/common.h>

uint64_t getJoltage(FileReader* reader, size_t batteryCount, size_t startOffset) {
	if (batteryCount <= 0) return 0;

	size_t digitIndex = startOffset;
	for (size_t i = digitIndex + 1; i < reader->readBufferLength - (batteryCount - 1); i++) {
		if (reader->readBuffer[i] <= reader->readBuffer[digitIndex])
			continue;
		digitIndex = i;
	}
	uint64_t joltage = (uint64_t)(reader->readBuffer[digitIndex] - '0');
	for (size_t i = 1; i < batteryCount; i++) {
		joltage *= 10;
	}

	return joltage + getJoltage(reader, batteryCount - 1, digitIndex + 1);
}

int main() {
	FileReader fileReader = FileReader_getInput();

	uint64_t joltageSum = 0;
	while (!fileReader.endReached) {
		FileReader_readUntilDelim(&fileReader, "\n");

		const uint64_t joltage = getJoltage(&fileReader, 12, 0);
		joltageSum += joltage;
	}

	printf("Max Total Joltage Possible: %llu\n", joltageSum);
}