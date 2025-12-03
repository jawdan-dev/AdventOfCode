#include <common/common.h>

int main() {
	FileReader fileReader = FileReader_getInput();

	uint64_t joltageSum = 0;
	while (!fileReader.endReached) {
		FileReader_readUntilDelim(&fileReader, "\n");

		size_t tensIndex = 0;
		for (size_t i = tensIndex + 1; i < fileReader.readBufferLength - 1; i++) {
			if (fileReader.readBuffer[i] <= fileReader.readBuffer[tensIndex])
				continue;
			tensIndex = i;
		}
		size_t digitIndex = tensIndex + 1;
		for (size_t i = digitIndex + 1; i < fileReader.readBufferLength; i++) {
			if (fileReader.readBuffer[i] <= fileReader.readBuffer[digitIndex])
				continue;
			digitIndex = i;
		}

		const uint64_t joltage = ((fileReader.readBuffer[tensIndex] - '0') * 10) + (fileReader.readBuffer[digitIndex] - '0');
		joltageSum += joltage;
	}

	printf("Max Total Joltage Possible: %llu\n", joltageSum);
}