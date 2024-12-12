#include <common/common.h>
#include <common/point.h>
#include <common/direction.h>

typedef long long ValueType;

#define CHECKED '*'
#define FINISHED '-'

#define MAP_MAX 512
char map[MAP_MAX][MAP_MAX];
unsigned char sides[MAP_MAX][MAP_MAX];
int width = 0, height = 0;

void calculateSides(const Point* point) {
	const char plotChar = map[point->x][point->y];

	// Skip if already checked / evaluated.
	if (plotChar == CHECKED || plotChar == FINISHED) return;
	map[point->x][point->y] = CHECKED;

	static const Direction checks[] = { Direction_North, Direction_East, Direction_South, Direction_West };
	for (int i = 0; i < ARRAY_SIZE(checks); i++) {
		const Point checkPoint = Point_addRaw(*point, Direction_toPoint(checks[i]));

		if (0 <= checkPoint.x && checkPoint.x < width && 0 <= checkPoint.y && checkPoint.y < height &&
			(map[checkPoint.x][checkPoint.y] == plotChar || map[checkPoint.x][checkPoint.y] == CHECKED)) {
			// Recurse.
			calculateSides(&checkPoint);
			continue;
		}

		// Mark fence side.
		sides[point->x][point->y] |= Direction_getFlag(checks[i]);
	}
}

const ValueType getPerimeterCost() {
	ValueType areaCount = 0, sideCount = 0;

	// Count area.
	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++)
			if (map[i][j] == CHECKED)
				areaCount++;

	// Count sides (greedy-esque).
	static const Direction checks[] = { Direction_North, Direction_East, Direction_South, Direction_West };
	for (int k = 0; k < ARRAY_SIZE(checks); k++) {
		// Check information.
		const bool isHorizontal = checks[k] == Direction_North || checks[k] == Direction_South;
		const unsigned char check = Direction_getFlag(checks[k]);

		for (int j = 0; j < (isHorizontal ? height : width); j++) {
			int sideLength = 0;
			for (int i = 0; i < (isHorizontal ? width : height); i++) {
				const unsigned char sideFlags = isHorizontal ? sides[i][j] : sides[j][i];
				if (sideFlags & check) {
					// Side started / continued.
					sideLength++;
				} else {
					// Gap in side found.
					if (sideLength) sideCount++;
					sideLength = 0;
				}
			}
			if (sideLength) sideCount++;
		}
	}

	// Return perimeter cost.
	return areaCount * sideCount;
}
void finishChecks() {
	// Reset.
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