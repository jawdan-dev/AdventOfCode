#include <common/common.h>

#define BOX_MAX 1000
typedef struct {
	int x, y, z;
	int circuitIndex;
} Box;
Box boxes[BOX_MAX];
size_t boxCount = 0;
int circuitUUIDCounter = 0;
bool connectionMade[BOX_MAX][BOX_MAX];

float distance(Box* a, Box* b) {
	float cx = b->x - a->x,
		  cy = b->y - a->y,
		  cz = b->z - a->z;
	return sqrtf(
		(cx * cx) +
		(cy * cy) +
		(cz * cz));
}

int main() {
	FileReader fileReader = FileReader_getInput();
	memset(connectionMade, 0, sizeof(connectionMade));

	while (!fileReader.endReached) {
		FileReader_readUntilDelim(&fileReader, "\n");
		sscanf(fileReader.readBuffer, "%i,%i,%i", &boxes[boxCount].x, &boxes[boxCount].y, &boxes[boxCount].z);
		boxes[boxCount].circuitIndex = circuitUUIDCounter++;
		boxCount++;
	}

	for (int connections = 0; connections < 1000; connections++) {
		float bestDistance = INFINITY;
		int bestI = -1, bestJ = -1;
		for (int i = 0; i < boxCount; i++) {
			for (int j = i + 1; j < boxCount; j++) {
				if (connectionMade[i][j])
					continue;
				float dist = distance(&boxes[i], &boxes[j]);
				if (bestDistance <= dist)
					continue;
				bestDistance = dist;
				bestI = i;
				bestJ = j;
			}
		}
		ASSERT(bestI != -1 && bestJ != -1, "this should be very unlikely");

		connectionMade[bestI][bestJ] = true;
		connectionMade[bestJ][bestI] = true;

		int fromCircuit = boxes[bestJ].circuitIndex,
			toCircuit = boxes[bestI].circuitIndex;
		for (int i = 0; i < boxCount; i++) {
			if (boxes[i].circuitIndex == fromCircuit) {
				boxes[i].circuitIndex = toCircuit;
			}
		}
	}

	uint64_t bestSizes[3] = {-1, -1, -1};
	for (int i = 0; i < circuitUUIDCounter; i++) {
		uint64_t count = 0;
		for (int j = 0; j < boxCount; j++) {
			if (boxes[j].circuitIndex == i) {
				count++;
			}
		}

		for (int j = 0; j < 3; j++) {
			if (bestSizes[j] == -1 || bestSizes[j] < count) {
				uint64_t temp = bestSizes[j];
				bestSizes[j] = count;
				count = temp;
			}
		}
	}
	uint64_t result = bestSizes[0] * bestSizes[1] * bestSizes[2];
	printf("Best: %llu * %llu * %llu => %llu\n", bestSizes[0], bestSizes[1], bestSizes[2], result);
}