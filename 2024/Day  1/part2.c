#include <common/common.h>

typedef unsigned long long ValueType;
QSORT_FUNCTION(valueSort, ValueType);

int main() {
	printf("\n\n\n");
	FileReader fileReader = FileReader_getInput();

#	define MAX_VALUES 1000
	ValueType leftValues[MAX_VALUES], rightValues[MAX_VALUES];
	size_t valueCount = 0;

	while (!fileReader.endReached) {
		FileReader_readUntilDelim(&fileReader, "\n");

		sscanf(fileReader.readBuffer, "%llu   %llu", &leftValues[valueCount], &rightValues[valueCount]);
		valueCount++;
	}

	// The naive approach will be fine for this.
	ValueType similarityTotal = 0;
	for (size_t i = 0; i < valueCount; i++) {
		size_t similarityCount = 0;
		for (size_t j = 0; j < valueCount; j++)
			if (leftValues[i] == rightValues[j])
				similarityCount++;

		similarityTotal += leftValues[i] * similarityCount;
	}

	printf("Similarity total: %llu\n", similarityTotal);
}