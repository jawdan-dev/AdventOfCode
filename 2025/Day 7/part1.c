#include <common/common.h>

#define MAP_MAX 512
char map[MAP_MAX][MAP_MAX];
int width = MAP_MAX, height = MAP_MAX;

int main() {
	FileReader fileReader = FileReader_getInput();
	FileReader_readMap(&fileReader, (char*)&map, MAP_MAX, MAP_MAX, &width, &height);

	uint64_t splitCount = 0;
	for (int row = 1; row < height; row++) {
		for (int col = 0; col < width; col++) {
			switch (map[col][row - 1]) {
				case 'S': {
					map[col][row] = '|';
				} break;
				case '|': {
					if (map[col][row] == '^') {
						if (col - 1 >= 0)
							map[col - 1][row] = '|';
						if (col + 1 < width)
							map[col + 1][row] = '|';
						splitCount++;
					} else {
						map[col][row] = '|';
					}
				} break;
			}
			printf("%c", map[col][row]);
		}
		printf("\n");
	}
	printf("Split Count: %llu\n", splitCount);
}