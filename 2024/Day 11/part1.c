#include <common/common.h>
#include <common/point.h>

typedef long long ValueType;

#define STONES_MAX 1000000
ValueType stones[STONES_MAX];
size_t stoneCount = 0;

// Naive approach -- recursive gets style points!!
void blink() {
	const size_t currentStoneCount = stoneCount;
	for (int i = 0; i < currentStoneCount; i++) {
		if (stones[i] == 0) {
			stones[i] = 1;
			continue;
		}
		//
		int digitCount = 0;
		ValueType temp = stones[i];
		while (temp > 0) {
			temp /= 10;
			digitCount++;
		}
		//
		if (digitCount % 2 == 1) {
			stones[i] *= 2024;
			continue;
		}
		//
		ValueType right = 0;
		ValueType place = 1;
		for (int j = digitCount / 2; j < digitCount; j++) {
			right += (stones[i] % 10) * place;
			place *= 10;
			stones[i] /= 10;
		}
		ASSERT(stoneCount < STONES_MAX, "Stone max exceeded.");
		stones[stoneCount++] = right;
	}
}

int main() {
	FileReader fileReader = FileReader_getInput();
	while (!fileReader.endReached) {
		FileReader_readUntilDelim(&fileReader, " ");
		sscanf(fileReader.readBuffer, "%lli", &stones[stoneCount]);
		stoneCount++;
	}
	//
	for (int i = 0; i < 25; i++) blink();
	printf("Stone count: %zu\n", stoneCount);
}