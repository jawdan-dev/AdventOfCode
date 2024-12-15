#include <common/common.h>
#include <common/point.h>
#include <common/direction.h>

#define MAP_MAX 512
char map[MAP_MAX][MAP_MAX];
int width = 0, height = 0;
#define MOVES_MAX 100000
char moves[MOVES_MAX];
int moveCount = 0;

Point robotPos;

const bool isEmptyOrPushable(const Point* pos) {
	if (pos->x < 0 || pos->x >= width || pos->y < 0 || pos->y >= height) return false;
	if (map[pos->x][pos->y] == '#') return false;
	return true;
}

void push(const Direction direction) {
	const Point posChange = Direction_toPoint(direction);
	const Point newPos = Point_add(&robotPos, &posChange);

	Point toPos = newPos;
	while (isEmptyOrPushable(&toPos) && map[toPos.x][toPos.y] == 'O')
		toPos = Point_addRaw(toPos, posChange);
	if (!isEmptyOrPushable(&toPos)) return;

	if (!Point_equal(&toPos, &newPos)) map[toPos.x][toPos.y] = map[newPos.x][newPos.y];
	map[newPos.x][newPos.y] = '@';
	map[robotPos.x][robotPos.y] = '.';
	robotPos = newPos;
}

int main() {
	FileReader fileReader = FileReader_getInput();
	while (!fileReader.endReached) {
		FileReader_readUntilDelim(&fileReader, "\n");
		if (fileReader.readBufferLength <= 0) break;
		width = fileReader.readBufferLength;
		ASSERT(width <= MAP_MAX, "Map width exceeded.");
		ASSERT(height < MAP_MAX, "Map height exceeded.");
		for (int i = 0; i < width; i++)
			map[i][height] = fileReader.readBuffer[i];
		(height)++;
	}
	//
	while (!fileReader.endReached) {
		FileReader_readUntilDelim(&fileReader, "\n");
		for (int i = 0; i < fileReader.readBufferLength; i++) moves[moveCount++] = fileReader.readBuffer[i];
	}
	//
	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++)
			if (map[i][j] == '@')
				robotPos = (Point) { .x = i, .y = j, };
	//
	for (int i = 0; i < moveCount; i++) {
		switch (moves[i]) {
			case '^': push(Direction_North); break;
			case '>': push(Direction_East); break;
			case 'v': push(Direction_South); break;
			case '<': push(Direction_West); break;
		}
	}
	//
	long long mapTotal = 0;
	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++)
			if (map[i][j] == 'O')
				mapTotal += (j * 100) + i;
	//
	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++)
			printf("%c ", map[i][j]);
		printf("\n");
	}
	printf("Map total : %lli\n", mapTotal);
}