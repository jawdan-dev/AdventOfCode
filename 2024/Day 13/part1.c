#include <common/common.h>
#include <common/point.h>

#define PRIZES_MAX 1000
typedef struct {
	int ax, ay;
	int bx, by;
	int x, y;
} Prize;
const int aCost = 3, bCost = 1;
Prize prizes[PRIZES_MAX];
int prizeCount = 0;

const int getCheapest(Prize* prize) {
	const int bMax = min(prize->x / prize->bx, prize->y / prize->by);
	for (int i = bMax; i >= 0; i--) {
		const int
			remainingX = prize->x - (prize->bx * i),
			remainingY = prize->y - (prize->by * i);

		if ((remainingX % prize->ax == 0) && (remainingY % prize->ay == 0) &&
			remainingX / prize->ax == remainingY / prize->ay) {
			return ((remainingX / prize->ax) * aCost) + (i * bCost);
		}
	}
	return -1;
}

int main() {
	FileReader fileReader = FileReader_getInput();
	while (!fileReader.endReached) {
		FileReader_readUntilDelim(&fileReader, "\n");
		sscanf(fileReader.readBuffer, "Button A: X%i, Y%i", &prizes[prizeCount].ax, &prizes[prizeCount].ay);
		FileReader_readUntilDelim(&fileReader, "\n");
		sscanf(fileReader.readBuffer, "Button B: X%i, Y%i", &prizes[prizeCount].bx, &prizes[prizeCount].by);
		FileReader_readUntilDelim(&fileReader, "\n");
		sscanf(fileReader.readBuffer, "Prize: X=%i, Y=%i", &prizes[prizeCount].x, &prizes[prizeCount].y);
		FileReader_readUntilDelim(&fileReader, "\n");
		prizeCount++;
	}
	//
	int minimumTokens = 0;
	for (int i = 0; i < prizeCount; i++) {
		const int cheapest = getCheapest(&prizes[i]);
		if (cheapest != -1) minimumTokens += cheapest;
	}
	//
	printf("Tokens required: %i\n", minimumTokens);
}