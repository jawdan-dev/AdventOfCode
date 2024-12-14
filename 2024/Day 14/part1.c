#include <common/common.h>
#include <common/point.h>

#define ROBOT_MAX 1000
typedef struct {
	Point pos;
	Point velocity;
} Robot;
Robot robots[ROBOT_MAX];
int robotCount = 0;
const Point roomSize = { .x = 101, .y = 103 };

int main() {
	FileReader fileReader = FileReader_getInput();
	while (!fileReader.endReached) {
		FileReader_readUntilDelim(&fileReader, "\n");
		sscanf(fileReader.readBuffer, "p=%i,%i v=%i,%i", &robots[robotCount].pos.x, &robots[robotCount].pos.y, &robots[robotCount].velocity.x, &robots[robotCount].velocity.y);
		robotCount++;
	}
	//
	const int simulateCount = 100;
	int quadrantCount[4] = { 0, 0, 0, 0 };
	for (int i = 0; i < robotCount; i++) {
		robots[i].pos.x += robots[i].velocity.x * simulateCount;
		robots[i].pos.y += robots[i].velocity.y * simulateCount;

		while (robots[i].pos.x < 0) robots[i].pos.x += roomSize.x;
		while (robots[i].pos.y < 0) robots[i].pos.y += roomSize.y;

		robots[i].pos.x %= roomSize.x;
		robots[i].pos.y %= roomSize.y;

		if (robots[i].pos.x == roomSize.x / 2) continue;
		if (robots[i].pos.y == roomSize.y / 2) continue;

		const int quadrant =
			((robots[i].pos.x > roomSize.x / 2) ? 1 : 0) +
			((robots[i].pos.y > roomSize.y / 2) ? 2 : 0);
		quadrantCount[quadrant]++;
	}
	//
	int quadrantTotal = quadrantCount[0];
	for (int i = 1; i < ARRAY_SIZE(quadrantCount); i++)
		quadrantTotal *= quadrantCount[i];
	//
	printf("Robots in quadrents : %i\n", quadrantTotal);
}