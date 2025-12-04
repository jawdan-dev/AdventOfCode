#include <common/common.h>

#define MAP_MAX 512
char map[MAP_MAX][MAP_MAX];
int width = MAP_MAX, height = MAP_MAX;

int getAdjacentCount(const int x, const int y) {
	const int fromX = max(x - 1, 0), toX = min(x + 1, width - 1),
			  fromY = max(y - 1, 0), toY = min(y + 1, height - 1);

	int count = 0;
	for (int i = fromX; i <= toX; i++) {
		for (int j = fromY; j <= toY; j++) {
			if (i == x && j == y)
				continue;
			if (map[i][j] != '.')
				count++;
		}
	}
	return count;
}

int main() {
	FileReader fileReader = FileReader_getInput();
	FileReader_readMap(&fileReader, (char*)&map, MAP_MAX, MAP_MAX, &width, &height);

	int totalRemoved = 0;

	while (true) {
		int removable = 0;
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				if (map[i][j] == '@' && getAdjacentCount(i, j) < 4) {
					map[i][j] = 'x';
					removable++;
				}
			}
		}

		if (removable <= 0) break;

		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				if (map[i][j] == 'x') {
					map[i][j] = '.';
				}
			}
		}
		totalRemoved += removable;
	}
	printf("Total Removed Papers: %i\n", totalRemoved);
}