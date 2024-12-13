#include <common/common.h>
#include <common/point.h>

typedef long long ValueType;

#define PRIZES_MAX 1000
typedef struct {
	ValueType ax, ay;
	ValueType bx, by;
	ValueType x, y;
} Prize;
const ValueType aCost = 3, bCost = 1;
Prize prizes[PRIZES_MAX];
int prizeCount = 0;

const ValueType getCheapest(Prize* prize) {
	// Holy linear math, Batman!
	const ValueType
		aNumerator = (prize->x * prize->by) - (prize->y * prize->bx),
		aDenominator = (prize->bx * prize->ay) - (prize->by * prize->ax),
		bNumerator = (prize->x * prize->ay) - (prize->y * prize->ax),
		bDenominator = (prize->ax * prize->by) - (prize->ay * prize->bx);
	if (llabs(aNumerator) % llabs(aDenominator) != 0 || llabs(bNumerator) % llabs(bDenominator) != 0) return -1;
	//
	const ValueType
		aCount = llabs(aNumerator / aDenominator),
		bCount = llabs(bNumerator / bDenominator);
	return (aCount * aCost) + (bCount * bCost);
}

int main() {
	FileReader fileReader = FileReader_getInput();
	while (!fileReader.endReached) {
		FileReader_readUntilDelim(&fileReader, "\n");
		sscanf(fileReader.readBuffer, "Button A: X%lli, Y%lli", &prizes[prizeCount].ax, &prizes[prizeCount].ay);
		FileReader_readUntilDelim(&fileReader, "\n");
		sscanf(fileReader.readBuffer, "Button B: X%lli, Y%lli", &prizes[prizeCount].bx, &prizes[prizeCount].by);
		FileReader_readUntilDelim(&fileReader, "\n");
		sscanf(fileReader.readBuffer, "Prize: X=%lli, Y=%lli", &prizes[prizeCount].x, &prizes[prizeCount].y);
		FileReader_readUntilDelim(&fileReader, "\n");
		prizeCount++;
	}
	//
	ValueType minimumTokens = 0;
	for (int i = 0; i < prizeCount; i++) {
		prizes[i].x += 10000000000000;
		prizes[i].y += 10000000000000;
		const ValueType cheapest = getCheapest(&prizes[i]);
		if (cheapest != -1) minimumTokens += cheapest;
	}
	//
	printf("Tokens required: %lli\n", minimumTokens);
}