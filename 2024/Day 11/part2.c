#include <common/common.h>

typedef long long ValueType;

typedef struct {
	ValueType stoneValue;
	int depth;
} MemoizationKey;
typedef ValueType MemoizationResult;
#include <common/memoization.h>

const size_t handleStone(const ValueType stoneValue, const int remainingDepth) {
	if (remainingDepth == 0) return 1;

	// Cache key.
	const MemoizationKey key = {
		.stoneValue = stoneValue,
		.depth = remainingDepth,
	};

	// Check cache.
	MemoizationResult result;
	if (Memoization_getResult(&key, &result)) return result;

	if (stoneValue == 0) {
		// Replace stone with a 1.
		result = handleStone(1, remainingDepth - 1);
	} else {
		// Get stone digit count.
		int digitCount = 0;
		ValueType temp = stoneValue;
		while (temp > 0) {
			temp /= 10;
			digitCount++;
		}

		// Check if even amount of digits.
		if (digitCount % 2 == 1) {
			// Multiply stone by 2024.
			result = handleStone(stoneValue * 2024, remainingDepth - 1);
		} else {
			// Split stone.
			ValueType left = stoneValue, right = 0, place = 1;
			for (int j = digitCount / 2; j < digitCount; j++) {
				right += (left % 10) * place;
				place *= 10;
				left /= 10;
			}
			result = handleStone(left, remainingDepth - 1) + handleStone(right, remainingDepth - 1);
		}
	}

	// Cache result.
	Memoization_add(&key, &result);
	return result;
}

int main() {
#	define STONES_MAX 100
	ValueType stones[STONES_MAX], stoneCount = 0;
	//
	FileReader fileReader = FileReader_getInput();
	while (!fileReader.endReached) {
		FileReader_readUntilDelim(&fileReader, " ");
		sscanf(fileReader.readBuffer, "%lli", &stones[stoneCount++]);
	}
	//
	ValueType stoneTotal = 0;
	for (int i = 0; i < stoneCount; i++)
		stoneTotal += handleStone(stones[i], 75);
	//
	printf("Stone count: %lli\n", stoneTotal);
}