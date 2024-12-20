#include <common/common.h>
#include <common/point.h>
#include <common/direction.h>

typedef long long ValueType;

#define MAP_MAX 512
char map[MAP_MAX][MAP_MAX];
ValueType visited[MAP_MAX][MAP_MAX];
ValueType endDistance[MAP_MAX][MAP_MAX];
int width = MAP_MAX, height = MAP_MAX;

typedef struct {
	Point pos;
	ValueType cost;
}  Node;
#define NODES_MAX 100000
#define CHEAT_MAX 20
Node nodes[NODES_MAX];
int nodeCount = 0;

const ValueType path(const Point start, const Point end) {
	nodeCount = 0;
	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++)
			visited[i][j] = -1;

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

			ValueType* const visit = &visited[checkPoint.x][checkPoint.y];
			if (*visit != -1 && *visit <= checkScore) continue;
			*visit = checkScore;

			nodes[nodeCount++] = (Node) {
				.pos = checkPoint,
				.cost = checkScore,
			};
		}
	}

	return -1;
}

int main() {
	FileReader fileReader = FileReader_getInput();
	FileReader_readMap(&fileReader, (char*)&map, MAP_MAX, MAP_MAX, &width, &height);
	//
	Point startPoint = { .x = 0, .y = 0 }, endPoint = { .x = 0, .y = 0 };
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			if (map[i][j] == 'S') {
				startPoint = (Point) { .x = i, .y = j };
				map[i][j] = '.';
			}
			if (map[i][j] == 'E') {
				endPoint = (Point) { .x = i, .y = j };
				map[i][j] = '.';
			}
		}
	}
	//
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			endDistance[i][j] = 0;
			if (map[i][j] != '.') continue;

			const Point distancePos = { .x = i, .y = j };
			endDistance[i][j] = path(distancePos, endPoint);
		}
	}
	//
	const ValueType cheatMax = endDistance[startPoint.x][startPoint.y];
	printf("Cheat max: %lli\n", cheatMax);
	//
	ValueType cheatCount = 0;
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			if (map[i][j] != '.') continue;
			const Point basePos = { .x = i, .y = j };

			const ValueType baseCost = path(startPoint, basePos);
			for (int k = 0; k < width; k++) {
				for (int l = 0; l < height; l++) {
					const Point wallOrigin = { .x = k, .y = l };
					if (map[wallOrigin.x][wallOrigin.y] != '.') continue;
					if (Point_equal(&wallOrigin, &basePos)) continue;

					const ValueType wallPathCost = abs(wallOrigin.x - basePos.x) + abs(wallOrigin.y - basePos.y);
					if (wallPathCost > CHEAT_MAX) continue;
					const ValueType endCost = endDistance[wallOrigin.x][wallOrigin.y];

					const ValueType pathLength = baseCost + wallPathCost + endCost;

					if (pathLength == cheatMax) continue;
					if ((cheatMax - pathLength) < 100) continue;
					cheatCount++;
				}
			}
			printf("(%i, %i)\r", basePos.x, basePos.y);
		}
	}
	printf("Total cheatables: %lli\n", cheatCount);
}