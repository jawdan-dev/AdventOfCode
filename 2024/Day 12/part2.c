#include <common/common.h>
#include <common/point.h>
#include <common/direction.h>

typedef long long ValueType;

#define MAP_MAX 512
char map[MAP_MAX][MAP_MAX];
unsigned char sides[MAP_MAX][MAP_MAX];
int width = 0, height = 0;

#define CHECKED '*'
#define FINISHED '-'

void calculateSides(const Point* point) {
	const char plotChar = map[point->x][point->y];
	if (plotChar == CHECKED || plotChar == FINISHED) return;
	map[point->x][point->y] = CHECKED;

	static const Direction checks[] = { Direction_North, Direction_East, Direction_South, Direction_West };
	for (int i = 0; i < ARRAY_SIZE(checks); i++) {
		const Point checkPoint = Point_addRaw(*point, Direction_toPoint(checks[i]));

		if (0 <= checkPoint.x && checkPoint.x < width && 0 <= checkPoint.y && checkPoint.y < height &&
			(map[checkPoint.x][checkPoint.y] == plotChar || map[checkPoint.x][checkPoint.y] == CHECKED)) {
			calculateSides(&checkPoint);
			continue;
		}

		sides[point->x][point->y] |= Direction_getFlag(checks[i]);
	}
}

const ValueType getPerimeterCost() {
	ValueType areaCount = 0, sideCount = 0;

	static const Direction checks[] = { Direction_North, Direction_East, Direction_South, Direction_West };

	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++)
			if (map[i][j] == CHECKED)
				areaCount++;

	static const Direction horizontalChecks[] = { Direction_North, Direction_South };
	for (int k = 0; k < ARRAY_SIZE(horizontalChecks); k++) {
		const unsigned char check = Direction_getFlag(horizontalChecks[k]);
		for (int j = 0; j < height; j++) {
			int sideLength = 0;
			for (int i = 0; i < width; i++) {
				if (sides[i][j] & check) {
					sideLength++;
				} else {
					if (sideLength) sideCount++;
					sideLength = 0;
				}
			}
			if (sideLength) sideCount++;
		}
	}

	static const Direction verticalChecks[] = { Direction_East, Direction_West };
	for (int k = 0; k < ARRAY_SIZE(horizontalChecks); k++) {
		const unsigned char check = Direction_getFlag(verticalChecks[k]);
		for (int i = 0; i < width; i++) {
			int sideLength = 0;
			for (int j = 0; j < height; j++) {
				if (sides[i][j] & check) {
					sideLength++;
				} else {
					if (sideLength) sideCount++;
					sideLength = 0;
				}
			}
			if (sideLength) sideCount++;
		}
	}

	return areaCount * sideCount;
}
void finishChecks() {
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			if (map[i][j] == CHECKED)
				map[i][j] = FINISHED;
			sides[i][j] = 0;
		}
	}
}

int main() {
	FileReader fileReader = FileReader_getInput();
	FileReader_readMap(&fileReader, (char*)map, MAP_MAX, MAP_MAX, &width, &height);
	//
	Point pos;
	ValueType perimeterCost = 0;
	for (pos.x = 0; pos.x < width; pos.x++) {
		for (pos.y = 0; pos.y < height; pos.y++) {
			calculateSides(&pos);
			perimeterCost += getPerimeterCost();
			finishChecks();
		}
	}
	//
	printf("Perimeter cost: %i\n", perimeterCost);
}