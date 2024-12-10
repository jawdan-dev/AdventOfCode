#include <common/common.h>
#include <common/point.h>

#define MAP_MAX 512
char map[MAP_MAX][MAP_MAX];
bool reachable[MAP_MAX][MAP_MAX];
int width = 0, height = 0;

#define NODES_MAX 1000
typedef struct {
	Point pos;
} Node;

const int startTrail(const Point startingPoint) {
	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++)
			reachable[i][j] = false;
	//
	static Node nodes[NODES_MAX];
	int nodeCount = 0;
	nodes[nodeCount++] = (Node) {
		.pos = startingPoint,
	};
	//
	while (nodeCount > 0) {
		const Node node = nodes[0];
		for (size_t i = 1; i < nodeCount; i++)
			nodes[i - 1] = nodes[i];
		nodeCount--;
		//
		char mapValue = map[node.pos.x][node.pos.y];
		if (mapValue == 9) {
			reachable[node.pos.x][node.pos.y] = true;
			continue;
		}
		//
		const char checkValue = mapValue + 1;
		static const Point checks[] = { {.x = +1, .y = +0 }, {.x = -1, .y = +0 }, {.x = +0, .y = +1 }, {.x = +0, .y = -1 } };
		for (int i = 0; i < ARRAY_SIZE(checks); i++) {
			const Point checkPoint = Point_add(&node.pos, &checks[i]);
			if (checkPoint.x < 0 || checkPoint.x >= width || checkPoint.y < 0 || checkPoint.y >= height) continue;
			if (map[checkPoint.x][checkPoint.y] != checkValue) continue;
			//
			const Node checkNode = { .pos = checkPoint };
			nodes[nodeCount++] = checkNode;
		}
	}
	//
	int foundCount = 0;
	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++)
			if (reachable[i][j]) foundCount++;
	return foundCount;
}

int main() {
	FileReader fileReader = FileReader_getInput();
	FileReader_readMap(&fileReader, (char*)map, MAP_MAX, MAP_MAX, &width, &height);
	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++)
			map[i][j] -= '0';
	//
	Point pos;
	int pathTotals = 0;
	for (pos.x = 0; pos.x < width; pos.x++)
		for (pos.y = 0; pos.y < height; pos.y++)
			if (map[pos.x][pos.y] == 0)
				pathTotals += startTrail(pos);
	//
	printf("Path total: %i\n", pathTotals);
}