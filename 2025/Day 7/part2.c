#include <common/common.h>

typedef struct {
	int col, row;
} MemoizationKey;
typedef uint64_t MemoizationResult;
#include <common/memoization.h>

#define MAP_MAX 512
char map[MAP_MAX][MAP_MAX];
int width = MAP_MAX, height = MAP_MAX;

const uint64_t timelineCount(int col, int row) {
	if (col < 0 || col >= width) return 0;
	if (row >= height) return 1;

	const MemoizationKey key = {
		.col = col,
		.row = row,
	};
	MemoizationResult result = 0;
	if (Memoization_getResult(&key, &result)) return result;

	if (map[col][row] == '^') {
		result += timelineCount(col - 1, row + 1);
		result += timelineCount(col + 1, row + 1);
	} else {
		result += timelineCount(col, row + 1);
	}

	Memoization_add(&key, &result);
	return result;
}

int main() {
	FileReader fileReader = FileReader_getInput();
	FileReader_readMap(&fileReader, (char*)&map, MAP_MAX, MAP_MAX, &width, &height);

	uint64_t splitCount = 0;
	for (int col = 0; col < width; col++) {
		switch (map[col][0]) {
			case 'S': {
				splitCount = timelineCount(col, 1);
			} break;
		}
	}
	printf("Split Count: %llu\n", splitCount);
}