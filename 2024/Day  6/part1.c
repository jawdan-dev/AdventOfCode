#include <common/common.h>
#include <common/direction.h>
#include <common/point.h>


int main() {
	printf("\n\n\n");
	FileReader fileReader = FileReader_getInput();

#	define MAP_MAX 512
	char map[MAP_MAX][MAP_MAX];
	int width = 0, height = 0;
	while (!fileReader.endReached) {
		FileReader_readUntilDelim(&fileReader, "\n");

		// Read map.
		width = fileReader.readBufferLength;
		strncpy(map[height++], fileReader.readBuffer, width);
	}

	// Setup.
	Point startingPoint = { .x = -1, .y = -1 };
	bool visited[MAP_MAX][MAP_MAX];
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			visited[j][i] = false;

			if (map[j][i] == '^')
				startingPoint = (Point) { .x = i, .y = j };
		}
	}
	ASSERT(startingPoint.x != -1, "No starting point found.");

	typedef struct {
		Direction direction;
		Point position;
	} Guard;
	Guard guard = {
		.direction = Direction_North,
		.position = startingPoint,
	};

	while (0 <= guard.position.x && guard.position.x < width &&
		0 <= guard.position.y && guard.position.y < height) {

		visited[guard.position.y][guard.position.x] = true;

		const Point d = Direction_toPoint(guard.direction);
		const Point nextPoint = Point_add(&guard.position, &d);
		if (0 <= nextPoint.x && nextPoint.x < width &&
			0 <= nextPoint.y && nextPoint.y < height &&
			map[nextPoint.y][nextPoint.x] == '#') {
			guard.direction = Direction_rotate(guard.direction, 1);
		} else {
			guard.position = nextPoint;
		}
	}

	int totalCovered = 0;
	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++)
			if (visited[j][i])
				totalCovered++;
	printf("Total covered: %i\n", totalCovered);
}