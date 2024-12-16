#include <common/common.h>
#include <common/point.h>
#include <common/direction.h>

typedef long long ValueType;

#define MAP_MAX 512
char map[MAP_MAX][MAP_MAX];
ValueType visited[MAP_MAX][MAP_MAX][Direction_Count];
int width = 0, height = 0;

#define PATHING_MAX 1000
typedef struct {
	Point pos;
	ValueType cost;
	Direction direction;
	Direction path[PATHING_MAX];
	int pathCount;
}  Node;
#define NODES_MAX 10000
Node nodes[NODES_MAX];
int nodeCount = 0;

const ValueType path(Point start, Point end) {
	nodeCount = 0;
	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++)
			for (int k = 0; k < Direction_Count; k++)
				visited[i][j][k] = 0;

	visited[start.x][start.y][Direction_East] = true;
	nodes[nodeCount++] = (Node) {
		.pos = start,
		.cost = 0,
		.direction = Direction_East,
		.pathCount = 0,
	};

#	define BESTPATH_MAX 100
	Direction bestPaths[BESTPATH_MAX][PATHING_MAX];
	int pathCounts[BESTPATH_MAX];
	int bestPathCount = 0;
	ValueType bestPathScore = -1;

	while (nodeCount > 0) {
		size_t bestNodeIndex = 0;
		for (size_t i = 1; i < nodeCount; i++)
			if (nodes[bestNodeIndex].cost > nodes[i].cost)
				bestNodeIndex = i;

		const Node node = nodes[bestNodeIndex];
		for (size_t i = bestNodeIndex + 1; i < nodeCount; i++)
			nodes[i - 1] = nodes[i];
		nodeCount--;

		if (Point_equal(&node.pos, &end)) {
			if (bestPathScore == -1 || node.cost <= bestPathScore) {
				if (bestPathScore == -1 || node.cost < bestPathScore) {
					bestPathCount = 0;
					bestPathScore = node.cost;
				}
				memcpy(bestPaths[bestPathCount], node.path, node.pathCount * sizeof(*node.path));
				pathCounts[bestPathCount] = node.pathCount;
				bestPathCount++;
			}
			continue;
		}

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

			if (visited[checkPoint.x][checkPoint.y][checks[i]] != 0 && visited[checkPoint.x][checkPoint.y][checks[i]] < checkScore) continue;
			visited[checkPoint.x][checkPoint.y][checks[i]] = checkScore;

			Node checkNode = {
				.pos = checkPoint,
				.cost = checkScore,
				.direction = checks[i],
				.pathCount = node.pathCount,
			};
			memcpy(checkNode.path, node.path, node.pathCount * sizeof(*node.path));
			ASSERT(checkNode.pathCount < PATHING_MAX, "Path max exceeded.");
			checkNode.path[checkNode.pathCount++] = checks[i];

			nodes[nodeCount++] = checkNode;
		}
	}
	//
	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++)
			for (int k = 0; k < Direction_Count; k++)
				visited[i][j][k] = 0;
	//
	for (int i = 0; i < bestPathCount; i++) {
		Point pos = start;
		visited[pos.x][pos.y][0]++;

		for (int j = 0; j < pathCounts[i]; j++) {
			pos = Point_addRaw(pos, Direction_toPoint(bestPaths[i][j]));
			visited[pos.x][pos.y][0]++;
		}
	}
	//
	int totalInvolved = 0;
	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++)
			if (visited[i][j][0])
				totalInvolved++;
	return totalInvolved;
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
	path(startPoint, endPoint);
	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++)
			printf("%c ", visited[i][j][0] ? 'O' : map[i][j]);
		printf("\n");
	}
	//
	printf("Total involved in path: %i\n", path(startPoint, endPoint));
}