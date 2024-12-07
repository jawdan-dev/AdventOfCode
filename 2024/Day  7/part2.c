#include <common/common.h>

typedef unsigned long long valueType;
const bool validTest(const valueType target, const valueType value, const valueType* remaining, const int remainingCount) {
	if (remainingCount == 0) return target == value;
	if (value > target) return false;

	// Setup test data.
	const int testCount = remainingCount - 1;
	const valueType* const testNumbers = remaining + 1;
	const valueType usedNumber = remaining[0];

	// Get || operator setup.
	valueType leftMultiplier = 1, temp = usedNumber;
	while (temp > 0) {
		temp /= 10;
		leftMultiplier *= 10;
	}

	// Run tests.
	const bool result =
		(value > 0 && validTest(target, value * usedNumber, testNumbers, testCount)) ||
		(value > 0 && validTest(target, (value * leftMultiplier) + usedNumber, testNumbers, testCount)) ||
		validTest(target, value + usedNumber, testNumbers, testCount);

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

		if (validTest(target, 0, testNumbers, testNumberCount))
			testTotal += target;
	}
	printf("Total covered: %llu\n", testTotal);
}