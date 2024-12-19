#include <common/common.h>

typedef unsigned long long ValueType;

#define PATTERN_MAX 300
#define DESIGNS_MAX 500
char designs[DESIGNS_MAX][PATTERN_MAX];
int designCount = 0;

typedef struct {
	char towel[PATTERN_MAX];
	ValueType towelLength;
} MemoizationKey;
typedef ValueType MemoizationResult;
#include <common/memoization.h>

const MemoizationResult validTowel(const char* towel, const int towelLength) {
	if (towelLength <= 0) return 1;

	// Memoize.
	MemoizationKey key;
	key.towelLength = towelLength;
	for (int i = 0; i < towelLength; i++) key.towel[i] = towel[i];
	for (int i = towelLength; i < PATTERN_MAX; i++) key.towel[i] = '\0';
	// Get cached value.
	MemoizationResult result = 0;
	if (Memoization_getResult(&key, &result)) return result;

	for (int i = 0; i < designCount; i++) {
		const char* const design = designs[i];
		const int len = strlen(design);
		if (towelLength < len) continue;

		bool matches = true;
		for (int j = 0; j < len && matches; j++)
			if (towel[j] != design[j])
				matches = false;
		if (!matches) continue;

		result += validTowel(towel + len, towelLength - len);
	}

	// Cache result.
	Memoization_add(&key, &result);

	return result;
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
	MemoizationResult validTowelCount = 0;
	while (!fileReader.endReached) {
		FileReader_readUntilDelim(&fileReader, "\n");
		validTowelCount += validTowel(fileReader.readBuffer, fileReader.readBufferLength);
	}
	//
	printf("Valid towels count: %lli\n", validTowelCount);
}