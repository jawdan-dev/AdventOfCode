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
	Point santa = { .x = MAP_OFFSET, .y = MAP_OFFSET }, 
		robot = { .x = MAP_OFFSET, .y = MAP_OFFSET };
	map[santa.x][santa.y]++;
	map[robot.x][robot.y]++;
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
		Point* const target = (i % 2 == 0) ? &santa : &robot;
		//
		target->x += change.x;
		target->y += change.y;
		ASSERT(0 <= target->x && target->x < MAP_MAX, "Target exceeded map x limits.");
		ASSERT(0 <= target->y && target->y < MAP_MAX, "Target exceeded map y limits.");
		//
		map[target->x][target->y]++;
	}
	//
	SafeType total = 0;
	for (int i = 0; i < MAP_MAX; i++)
		for (int j = 0; j < MAP_MAX; j++)
			if (map[i][j] >= 1)
				total++;
	printf("Total visited at least once: %lli\n", total);
}