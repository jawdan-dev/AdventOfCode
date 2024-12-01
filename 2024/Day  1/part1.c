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

	qsort(leftValues, valueCount, sizeof(ValueType), &valueSort);
	qsort(rightValues, valueCount, sizeof(ValueType), &valueSort);

	ValueType differenceTotal = 0;
	for (size_t i = 0; i < valueCount; i++)
		if (rightValues[i] >= leftValues[i])
			differenceTotal += abs(rightValues[i] - leftValues[i]);
		else
			differenceTotal += abs(leftValues[i] - rightValues[i]);

	printf("Difference total: %llu\n", differenceTotal);
}