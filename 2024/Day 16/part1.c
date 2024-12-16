#include <common/common.h>
#include <common/point.h>
#include <common/direction.h>

typedef long long ValueType;

#define MAP_MAX 512
char map[MAP_MAX][MAP_MAX];
ValueType visited[MAP_MAX][MAP_MAX];
int width = 0, height = 0;

typedef struct {
	Point pos;
	ValueType cost;
	Direction direction;
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
		.direction = Direction_East,
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

		static const Direction checks[] = { Direction_North, Direction_East, Direction_South, Direction_West };
		for (int i = 0; i < ARRAY_SIZE(checks); i++) {
			const Point checkPoint = Point_addRaw(node.pos, Direction_toPoint(checks[i]));
			if (checkPoint.x < 0 || checkPoint.x >= width || checkPoint.y < 0 || checkPoint.y >= height) continue;
			if (map[checkPoint.x][checkPoint.y] == '#') continue;

			ValueType checkScore = node.cost + 1;
			if (checks[i] != node.direction && node.direction != Direction_None) {
				if (Direction_flip(node.direction) == checks[i]) continue;
				checkScore += 1000;
			}

			if (visited[checkPoint.x][checkPoint.y]) continue;
			visited[checkPoint.x][checkPoint.y] = true;

			nodes[nodeCount++] = (Node) {
				.pos = checkPoint,
				.cost = checkScore,
				.direction = checks[i],
			};
		}
	}

	return -1;
}

int main() {
	FileReader fileReader = FileReader_getInput();
	FileReader_readMap(&fileReader, (char*)&map, MAP_MAX, MAP_MAX, &width, &height);
	//
	Point startPoint, endPoint;
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			if (map[i][j] == 'S') {
				startPoint = (Point) { .x = i, .y = j, };
				map[i][j] = '.';
			}
			if (map[i][j] == 'E') {
				endPoint = (Point) { .x = i, .y = j, };
				map[i][j] = '.';
			}
		}
	}
	//
	printf("Shortest path: %i\n", path(startPoint, endPoint));
}