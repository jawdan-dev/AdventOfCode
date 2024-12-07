#include <common/common.h>

typedef unsigned long long valueType;
const bool validTest(const valueType target, const valueType value, const valueType* remaining, const int remainingCount) {
	if (remainingCount == 0) return target == value;
	if (value > target) return false;

	const int testCount = remainingCount - 1;
	valueType* testNumbers = malloc(sizeof(valueType) * testCount);

	// Copy numbers.
	for (int j = 0; j < testCount; j++)
		testNumbers[j] = remaining[j + 1];
	const valueType usedNumber = remaining[0];

	// Run tests.
	const bool result =
		(value > 0 && validTest(target, value * usedNumber, testNumbers, testCount)) ||
		validTest(target, value + usedNumber, testNumbers, testCount);

	free(testNumbers);

	return result;
}

int main() {
	printf("\n\n\n");
	FileReader fileReader = FileReader_getInput();

	valueType testTotal = 0;

#	define TEST_MAX 20
	valueType testNumbers[TEST_MAX];
	valueType testNumberCount = 0;
	while (!fileReader.endReached) {
		FileReader_readUntilDelim(&fileReader, "\n");

		valueType target;
		FileReader_readNextLLU(&fileReader, &target);
		testNumberCount = 0;
		while (FileReader_readNextLLU(&fileReader, &testNumbers[testNumberCount])) testNumberCount++;

		if (validTest(target, 0, testNumbers, testNumberCount)) {
			testTotal += target;
			printf("%llu was successful * \n", target);
		} else {
			printf("%llu was unsuccessful\n", target);
		}
	}

	printf("Total covered: %llu\n", testTotal);
}