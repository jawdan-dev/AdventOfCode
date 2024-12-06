#include <common/common.h>
#include <common/direction.h>
#include <common/point.h>


#define MAP_MAX 256
static char map[MAP_MAX][MAP_MAX];
static int width = 0, height = 0;
static unsigned int visited[MAP_MAX][MAP_MAX];

const bool checkForRepeatRecursive(const Point position, const Direction direction) {
	if (position.x < 0 || position.x >= width || position.y < 0 || position.y >= height) return false;

	// Check if already visited.
	const unsigned char directionFlag = Direction_getFlag(direction);
	if (visited[position.y][position.x] & directionFlag) return true;
	if (visited[position.y][position.x] & (directionFlag << 4)) return true;

	// Mark as visited.
	visited[position.y][position.x] |= directionFlag << 4;

	// Get next point.
	const Point nextPoint = Point_addRaw(position, Direction_toPoint(direction));
	if (nextPoint.x < 0 || nextPoint.x >= width || nextPoint.y < 0 || nextPoint.y >= height) return false;

	// March.
	if (map[nextPoint.y][nextPoint.x] == '#') return checkForRepeatRecursive(position, Direction_rotate(direction, 1));
	return checkForRepeatRecursive(nextPoint, direction);
}

void marchGuardRecursive(const Point position, const Direction direction) {
	if (position.x < 0 || position.x >= width || position.y < 0 || position.y >= height) return;

	// Mark as visited.
	const unsigned char directionFlag = Direction_getFlag(direction);
	visited[position.y][position.x] |= directionFlag;

	// Get next point.
	const Point nextPoint = Point_addRaw(position, Direction_toPoint(direction));
	if (nextPoint.x < 0 || nextPoint.x >= width || nextPoint.y < 0 || nextPoint.y >= height) return;

	// Rotate & march.
	if (map[nextPoint.y][nextPoint.x] == '#')
		return marchGuardRecursive(position, Direction_rotate(direction, 1));

	// Make sure placing an obstacle doesn't break the guard's path.
	if (!visited[nextPoint.y][nextPoint.x]) {
		// Check if potential obstruction would work.
		map[nextPoint.y][nextPoint.x] = '#';
		if (checkForRepeatRecursive(position, Direction_rotate(direction, 1)))
			visited[nextPoint.y][nextPoint.x] |= 0b1 << 8;

		// Cleanup.
		map[nextPoint.y][nextPoint.x] = '.';
		for (int i = 0; i < width; i++)
			for (int j = 0; j < height; j++)
				visited[j][i] &= ~(0b1111 << 4);
	}

	// March.
	marchGuardRecursive(nextPoint, direction);
}

int main() {
	printf("\n\n\n");
	FileReader fileReader = FileReader_getInput();

	while (!fileReader.endReached) {
		FileReader_readUntilDelim(&fileReader, "\n");

		// Read map.
		width = fileReader.readBufferLength;
		strncpy(map[height++], fileReader.readBuffer, width);
	}

	// Setup.
	Point startingPoint = { .x = -1, .y = -1 };
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			visited[j][i] = 0;

			if (map[j][i] == '^')
				startingPoint = (Point) { .x = i, .y = j };
		}
	}
	ASSERT(startingPoint.x != -1, "No starting point found.");

	marchGuardRecursive(startingPoint, Direction_North);
	visited[startingPoint.y][startingPoint.x] = 0;

	int potentialObstructionCount = 0;
	for (int j = 0; j < height; j++)
		for (int i = 0; i < width; i++)
			if (visited[j][i] & (0b1 << 8))
				potentialObstructionCount++;
	printf("Potential obstructions: %i\n", potentialObstructionCount);
}