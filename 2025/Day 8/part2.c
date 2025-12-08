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

	int bestI = -1, bestJ = -1;
	for (int connections = 1; connections < boxCount; connections++) {
		float bestDistance = INFINITY;
		bestI = -1;
		bestJ = -1;
		for (int i = 0; i < boxCount; i++) {
			for (int j = i + 1; j < boxCount; j++) {
				if (boxes[i].circuitIndex == boxes[j].circuitIndex)
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
	printf("Result: %i\n", boxes[bestI].x * boxes[bestJ].x);
}