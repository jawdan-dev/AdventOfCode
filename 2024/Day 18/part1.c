#include <common/common.h>
#include <common/point.h>
#include <common/direction.h>

typedef long long ValueType;

#define MAP_MAX 71
char map[MAP_MAX][MAP_MAX];
bool visited[MAP_MAX][MAP_MAX];
int width = MAP_MAX, height = MAP_MAX;

typedef struct {
	Point pos;
	ValueType cost;
}  Node;
#define NODES_MAX 100000
Node nodes[NODES_MAX];
int nodeCount = 0;

const ValueType path(Point start, Point end) {
	nodeCount = 0;
	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++)
			visited[i][j] = false;

	nodes[nodeCount++] = (Node) {
		.pos = start,
		.cost = 0,
	};

	while (nodeCount > 0) {
		size_t bestNodeIndex = 0;
		for (size_t i = 1; i < nodeCount; i++)
			if (nodes[bestNodeIndex].cost > nodes[i].cost)
				bestNodeIndex = i;

		const Node node = nodes[bestNodeIndex];
		for (size_t i = bestNodeIndex + 1; i < nodeCount; i++)
			nodes[i - 1] = nodes[i];
		nodeCount--;

		if (Point_equal(&node.pos, &end))
			return node.cost;

		const ValueType checkScore = node.cost + 1;
		static const Direction checks[] = { Direction_North, Direction_East, Direction_South, Direction_West };
		for (int i = 0; i < ARRAY_SIZE(checks); i++) {
			const Point checkPoint = Point_addRaw(node.pos, Direction_toPoint(checks[i]));
			if (checkPoint.x < 0 || checkPoint.x >= width || checkPoint.y < 0 || checkPoint.y >= height) continue;
			if (map[checkPoint.x][checkPoint.y] == '#') continue;

			if (visited[checkPoint.x][checkPoint.y]) continue;
			visited[checkPoint.x][checkPoint.y] = true;

			nodes[nodeCount++] = (Node) {
				.pos = checkPoint,
				.cost = checkScore,
			};
		}
	}

	return -1;
}

int main() {
	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++)
			map[i][j] = '.';
	//
	int x, y;
	FileReader fileReader = FileReader_getInput();
	for (int i = 0; i < 1024 && !fileReader.endReached; i++) {
		FileReader_readUntilDelim(&fileReader, "\n");
		sscanf(fileReader.readBuffer, "%i,%i", &x, &y);
		map[x][y] = '#';
	}
	//
	const Point startPoint = { .x = 0, .y = 0 }, endPoint = { .x  = width - 1, .y = height - 1 };
	printf("Shortest path: %lli\n", path(startPoint, endPoint));
}