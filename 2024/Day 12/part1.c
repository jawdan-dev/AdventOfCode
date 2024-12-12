#include <common/common.h>
#include <common/point.h>

typedef long long ValueType;

#define MAP_MAX 512
char map[MAP_MAX][MAP_MAX];
int width = 0, height = 0;

#define CHECKED '*'
#define FINISHED '-'

void calculatePerimeterCost(const Point* point, ValueType* const areaCount, ValueType* const perimeterCount) {
	const char plotChar = map[point->x][point->y];
	if (plotChar == CHECKED || plotChar == FINISHED) return;
	map[point->x][point->y] = CHECKED;

	(*areaCount)++;

	static const Point checks[] = { {.x = +1, .y = +0 }, {.x = -1, .y = +0 }, {.x = +0, .y = +1 }, {.x = +0, .y = -1 } };
	for (int i = 0; i < ARRAY_SIZE(checks); i++) {
		const Point checkPoint = Point_add(point, &checks[i]);
		if (0 <= checkPoint.x && checkPoint.x < width && 0 <= checkPoint.y && checkPoint.y < height &&
			(map[checkPoint.x][checkPoint.y] == plotChar || map[checkPoint.x][checkPoint.y] == CHECKED)) {
			calculatePerimeterCost(&checkPoint, areaCount, perimeterCount);
			continue;
		}
		(*perimeterCount)++;
	}
}

void finishChecks() {
	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++)
			if (map[i][j] == CHECKED)
				map[i][j] = FINISHED;
}

int main() {
	FileReader fileReader = FileReader_getInput();
	FileReader_readMap(&fileReader, (char*)map, MAP_MAX, MAP_MAX, &width, &height);
	//
	Point pos;
	ValueType perimeterCost = 0;
	for (pos.x = 0; pos.x < width; pos.x++) {
		for (pos.y = 0; pos.y < height; pos.y++) {
			ValueType areaCount = 0, perimeterCount = 0;
			calculatePerimeterCost(&pos, &areaCount, &perimeterCount);
			perimeterCost += areaCount * perimeterCount;
			finishChecks();
		}
	}
	//
	printf("Perimeter cost: %i\n", perimeterCost);
}