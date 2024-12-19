#include <common/common.h>

#define PATTERN_MAX 300
#define DESIGNS_MAX 500
char designs[DESIGNS_MAX][PATTERN_MAX];
int designCount = 0;

const bool validTowel(const char* towel, const int towelLength) {
	if (towelLength <= 0) return true;

	for (int i = 0; i < designCount; i++) {
		const char* const design = designs[i];
		const int len = strlen(design);
		if (towelLength < len) continue;

		bool matches = true;
		for (int j = 0; j < len && matches; j++)
			if (towel[j] != design[j])
				matches = false;
		if (!matches) continue;

		if (validTowel(towel + len, towelLength - len))
			return true;
	}
	return false;
}

int main() {
	for (int i = 0; i < DESIGNS_MAX; i++)
		for (int j = 0; j < PATTERN_MAX; j++)
			designs[i][j] = '\0';
	//
	FileReader fileReader = FileReader_getInput();
	FileReader_readUntilDelim(&fileReader, "\n");
	for (int i = 0; i <= fileReader.readBufferLength; i++) {
		const char c = fileReader.readBuffer[i];
		if (c == ',' || c == ' ' || c == '\0') {
			if (strlen(designs[designCount]) > 0) designCount++;
			continue;
		}
		designs[designCount][strlen(designs[designCount])] = c;
	}
	printf("Design count: %i\n", designCount);
	//
	FileReader_readUntilDelim(&fileReader, "\n");
	//
	int validTowelCount = 0;
	while (!fileReader.endReached) {
		FileReader_readUntilDelim(&fileReader, "\n");

		if (validTowel(fileReader.readBuffer, fileReader.readBufferLength))
			validTowelCount++;
	}
	//
	printf("Valid towels: %i\n", validTowelCount);
}