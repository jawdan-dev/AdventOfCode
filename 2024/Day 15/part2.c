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
const bool canPushBox(const Point pos, const int yFactor) {
	if (pos.x < 0 || pos.x >= width || pos.y < 0 || pos.y >= height) return false;
	if (map[pos.x][pos.y] == ']') {
		Point p = pos;
		p.x -= 1;
		return canPushBox(p, yFactor);
	}
	if (map[pos.x][pos.y] != '[' || map[pos.x + 1][pos.y] != ']') return false;

	Point next = pos;
	next.y += yFactor;

	const char
		leftChar = map[next.x][next.y],
		rightChar = map[next.x + 1][next.y];
	if (leftChar == '#' || rightChar == '#') return false;

	if (leftChar == '[') return canPushBox(next, yFactor);

	bool canPush = true;
	if (leftChar == ']' && !canPushBox(Point_addRaw(next, (Point) {
		.x = -1, .y = 0
	}), yFactor)) canPush = false;
	if (rightChar == '[' && !canPushBox(Point_addRaw(next, (Point) {
		.x = +1, .y = 0
	}), yFactor)) canPush = false;
	return canPush;
}
void pushBox(const Point pos, const int yFactor) {
	if (!canPushBox(pos, yFactor)) return;
	if (map[pos.x][pos.y] == ']') {
		Point p = pos;
		p.x -= 1;
		return pushBox(p, yFactor);
	}

	Point next = pos;
	next.y += yFactor;
	pushBox(next, yFactor);
	next.x += 1;
	pushBox(next, yFactor);

	map[pos.x][pos.y + yFactor] = '[';
	map[pos.x + 1][pos.y + yFactor] = ']';
	map[pos.x][pos.y] = '.';
	map[pos.x + 1][pos.y] = '.';
}

void push(const Direction direction) {
	const Point posChange = Direction_toPoint(direction);
	const Point newPos = Point_add(&robotPos, &posChange);

	Point toPos = newPos;
	switch (direction) {
		case Direction_North:
		case Direction_South: {
			if (map[toPos.x][toPos.y] != '.') {
				const int yFactor = direction == Direction_North ? -1 : 1;
				if (!canPushBox(toPos, yFactor)) return;
				pushBox(toPos, yFactor);
			}
		} break;

		case Direction_East:
		case Direction_West: {
			while (isEmptyOrPushable(&toPos) && (map[toPos.x][toPos.y] == '[' || map[toPos.x][toPos.y] == ']'))
				toPos = Point_addRaw(toPos, posChange);
			if (!isEmptyOrPushable(&toPos)) return;

			// Push right.
			// ##...[][]..

			if (direction == Direction_East) {
				for (int i = toPos.x; i > robotPos.x; i--)
					map[i][robotPos.y] = map[i - 1][robotPos.y];
			} else {
				for (int i = toPos.x; i < robotPos.x; i++)
					map[i][robotPos.y] = map[i + 1][robotPos.y];
			}
			map[newPos.x][newPos.y] = '.';
		} break;
	}

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
	map[robotPos.x][robotPos.y] = '.';
	//
	for (int j = 0; j < height; j++) {
		for (int i = width - 1; i >= 0; i--) {
			const int newLeft = (i * 2);
			const int newRight = newLeft + 1;
			switch (map[i][j]) {
				case '@': map[i][j] = '.';
				default: {
					map[newRight][j] = map[i][j];
					map[newLeft][j] = map[i][j];
				} break;
				case 'O': {
					map[newRight][j] = ']';
					map[newLeft][j] = '[';
				} break;
			}
		}
	}
	robotPos.x *= 2;
	width *= 2;
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
			if (map[i][j] == '[')
				mapTotal += (j * 100) + i;
	//
	map[robotPos.x][robotPos.y] = '@';
	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++)
			printf("%c ", map[i][j]);
		printf("\n");
	}
	printf("Map total : %lli\n", mapTotal);
}