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
	int quadrantCount[4] = { 0, 0, 0, 0 };
	int maxConsecutive = 0;
	int seconds = 0;
	while (maxConsecutive < 30) {
		int consecutive = 0;

		for (int i = 0; i < robotCount; i++) {
			robots[i].pos.x += robots[i].velocity.x;
			robots[i].pos.y += robots[i].velocity.y;

			while (robots[i].pos.x < 0) robots[i].pos.x += roomSize.x;
			while (robots[i].pos.y < 0) robots[i].pos.y += roomSize.y;

			robots[i].pos.x %= roomSize.x;
			robots[i].pos.y %= roomSize.y;
		}

		for (int y = 0; y < roomSize.y; y++) {
			int cons = 0;
			for (int x = 0; x < roomSize.x; x++) {
				bool consFound = false;
				for (int i = 0; i < robotCount && !consFound; i++)
					if (robots[i].pos.x == x && robots[i].pos.y == y)
						consFound = true;

				if (consFound) {
					cons++;
				} else if (cons) {
					consecutive = max(consecutive, cons);;
					cons = 0;
				}
			}
			if (cons) consecutive = max(consecutive, cons);
		}

		maxConsecutive = max(maxConsecutive, consecutive);
		seconds++;
	}
	//
	int quadrantTotal = quadrantCount[0];
	for (int i = 1; i < ARRAY_SIZE(quadrantCount); i++)
		quadrantTotal *= quadrantCount[i];
	//
	printf("Seconds : %i\n", seconds);
}