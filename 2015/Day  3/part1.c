#include <common/common.h>
#include <common/point.h>

#define MAP_OFFSET 500
#define MAP_MAX 1000
int map[MAP_MAX][MAP_MAX];

int main() {
	FileReader fileReader = FileReader_getInput();
	FileReader_readUntilDelim(&fileReader, "\n");
	//
	for (int i = 0; i < MAP_MAX; i++)
		for (int j = 0; j < MAP_MAX; j++)
			map[i][j] = 0;
	//
	Point santa = { .x = MAP_OFFSET, .y = MAP_OFFSET };
	map[santa.x][santa.y]++;
	//
	for (int i = 0; i < fileReader.readBufferLength; i++) {
		Point change = { .x = 0, .y = 0 };
		switch (fileReader.readBuffer[i]) {
			case '^': change.y = -1; break;
			case '>': change.x = +1; break;
			case 'v': change.y = +1; break;
			case '<': change.x = -1; break;
		}
		//
		santa.x += change.x;
		santa.y += change.y;
		ASSERT(0 <= santa.x && santa.x < MAP_MAX, "Santa exceeded map limits.");
		ASSERT(0 <= santa.y && santa.y < MAP_MAX, "Santa exceeded map limits.");
		//
		map[santa.x][santa.y]++;
	}
	//
	SafeType total = 0;
	for (int i = 0; i < MAP_MAX; i++)
		for (int j = 0; j < MAP_MAX; j++)
			if (map[i][j] >= 1)
				total++;
	printf("Total visited at least once: %lli\n", total);
}