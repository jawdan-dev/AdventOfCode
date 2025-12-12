#include <common/common.h>

#define DATA_COUNT 9
#define ORIENTATION_COUNT 4
#define BOX_COUNT 6
#define STATE_MAX 64

typedef struct {
	char data[DATA_COUNT];
} BoxData;
typedef struct {
	BoxData orientation[ORIENTATION_COUNT];
	int squareCount;
} Box;

Box boxes[BOX_COUNT];

typedef struct {
	int width, height;
	int remainingCounts[BOX_COUNT];
	bool state[STATE_MAX][STATE_MAX];
} MemoizationKey;
typedef bool MemoizationResult;
#include <common/memoization.h>

int width, height;
int remainingCounts[BOX_COUNT];
bool state[STATE_MAX][STATE_MAX];
bool canFitFirst(int previousX, int previousY) {
	int box = -1;
	for (int i = 0; i < BOX_COUNT && box == -1; i++)
		if (remainingCounts[i] > 0)
			box = i;
	if (box == -1)
		return true;

	// MemoizationKey key;
	// key.width = width;
	// key.height = height;
	// memcpy(key.remainingCounts, remainingCounts, sizeof(remainingCounts));
	// memcpy(key.state, state, sizeof(state));
	MemoizationResult result = false;
	// if (Memoization_getResult(&key, &result))
	// 	return result;

	static const int xMap[DATA_COUNT] = {0, 1, 2, 0, 1, 2, 0, 1, 2},
					 yMap[DATA_COUNT] = {0, 0, 0, 1, 1, 1, 2, 2, 2};

	int x = previousX;
	for (int y = previousY; y < height - 2; y++, x = 0) {
		for (; x < width - 2; x++) {
			if (remainingCounts[box] <= 0)
				continue;
			for (int orientation = 0; orientation < ORIENTATION_COUNT; orientation++) {
				bool canPlace = true;
				for (int di = 0; di < DATA_COUNT && canPlace; di++)
					if (boxes[di].orientation[orientation].data[di] == '#' && state[x + xMap[di]][y + yMap[di]])
						canPlace = false;
				if (!canPlace)
					continue;

				for (int di = 0; di < DATA_COUNT; di++)
					if (boxes[di].orientation[orientation].data[di] == '#')
						state[x + xMap[di]][y + yMap[di]] = true;
				remainingCounts[box]--;

				if ((remainingCounts[box] > 0 && canFitFirst(x, y)) || canFitFirst(0, 0)) {
					result = true;
					// Memoization_add(&key, &result);
					return result;
				}

				for (int di = 0; di < DATA_COUNT; di++)
					if (boxes[di].orientation[orientation].data[di] == '#')
						state[x + xMap[di]][y + yMap[di]] = false;
				remainingCounts[box]++;
			}
		}
	}
	// Memoization_add(&key, &result);
	return result;
}

int main() {
	FileReader fileReader = FileReader_getInput();
	memset(state, 0, sizeof(state));

	{
		const int mapping[DATA_COUNT] = {2, 5, 8, 1, 4, 7, 0, 3, 6};
		for (int i = 0; i < BOX_COUNT; i++) {
			FileReader_readUntilDelim(&fileReader, "\n");
			for (int j = 0; j < 3; j++) {
				int ioff = j * 3;
				FileReader_readUntilDelim(&fileReader, "\n");
				sscanf(fileReader.readBuffer, "%c%c%c",
					   &boxes[i].orientation[0].data[ioff + 0],
					   &boxes[i].orientation[0].data[ioff + 1],
					   &boxes[i].orientation[0].data[ioff + 2]);
			}
			for (int j = 1; j < ORIENTATION_COUNT; j++) {
				for (int k = 0; k < DATA_COUNT; k++) {
					boxes[i].orientation[j].data[k] =
						boxes[i].orientation[j - 1].data[mapping[k]];
				}
			}
			boxes[i].squareCount = 0;
			for (int j = 0; j < DATA_COUNT; j++) {
				if (boxes[i].orientation[0].data[j] == '#')
					boxes[i].squareCount++;
			}
			FileReader_readUntilDelim(&fileReader, "\n");
		}
	}
	int count = 0;
	while (!fileReader.endReached) {
		printf("finished\n");
		FileReader_readUntilDelim(&fileReader, ":");
		sscanf(fileReader.readBuffer, "%ix%i", &width, &height);
		FileReader_readUntilDelim(&fileReader, " ");

		for (int i = 0; i < BOX_COUNT; i++) {
			FileReader_readUntilDelim(&fileReader, " \n");
			sscanf(fileReader.readBuffer, " %i", &remainingCounts[i]);
		}

		ASSERT(width < STATE_MAX && height < STATE_MAX, "Increase size, thank");
		printf("%ix%i ", width, height);

		int requiredSquares = 0;
		for (int i = 0; i < BOX_COUNT; i++) {
			requiredSquares += boxes[i].squareCount * remainingCounts[i];
		}
		if (requiredSquares > width * height)
			continue;

		// if (!canFitFirst(0, 0))
			// continue;
		// I actually hate it here.

		count++;
	}
	printf("Fit Count: %i\n", count);
}