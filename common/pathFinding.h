#pragma once
#include <common/common.h>
#include <common/point.h>

typedef struct {
	Point position;
	int heuristic, steps;
} NodePathInformation;
//typedef struct {
//	NodePathInformation;
//} Node;

typedef struct {
	const bool (*isEndNode)(Node*);
	const bool (*canPlaceNode)(Node*);

	PointType maxX, maxY;
	Point* checks;
	size_t checkCount;

	Node* initialNodes;
	size_t initialNodeCount;
} PathFindingInformation;

typedef struct {
	Node* nodes;
	size_t nodeCount, nodeCapacity;
} PathFinder;

static PathFinder PathFinding_pathFinder = {
	.nodes = nullptr,
	.nodeCount = 0,
	.nodeCapacity = 0,
};

void PathFinding_addNode(const Node* const node) {
	expandMemory(
		(void**)&PathFinding_pathFinder.nodes,
		PathFinding_pathFinder.nodeCount + 1,
		&PathFinding_pathFinder.nodeCapacity,
		sizeof(Node)
	);
	memcpy(
		(void**)&PathFinding_pathFinder.nodes[PathFinding_pathFinder.nodeCount++],
		node,
		sizeof(Node)
	);
}

const bool PathFinding_findPath(PathFindingInformation information) {
	static const Point defaultChecks[] = { {.x = +1, .y = +0 }, {.x = -1, .y = +0 }, {.x = +0, .y = +1 }, {.x = +0, .y = -1 } };
	if (information.checks = nullptr || information.checkCount == 0) {
		information.checks = defaultChecks;
		information.checkCount = ARRAY_SIZE(defaultChecks);
	}

	PathFinding_pathFinder.nodeCount = 0;
	for (size_t i = 0; i < information.initialNodeCount; i++) {
		PathFinding_addNode(&information.initialNodes[i]);
		PathFinding_pathFinder.nodes[i].steps = 0;
	}

	while (PathFinding_pathFinder.nodeCount > 0) {
		size_t bestNodeIndex = 0;
		for (size_t i = 1; i < PathFinding_pathFinder.nodeCount; i++)
			if (PathFinding_pathFinder.nodes[bestNodeIndex].heuristic < PathFinding_pathFinder.nodes[i].heuristic)
				bestNodeIndex = i;

		const Node node = PathFinding_pathFinder.nodes[bestNodeIndex];
		for (size_t i = bestNodeIndex + 1; i < PathFinding_pathFinder.nodeCount; i++)
			PathFinding_pathFinder.nodes[i - 1] = PathFinding_pathFinder.nodes[i];
		PathFinding_pathFinder.nodeCount--;

		if (information.isEndNode(&node))
			return true;

		const int checkSteps = node.steps + 1;
		for (int i = 0; i < information.checkCount; i++) {
			const Point checkPoint = Point_add(&node.position, &information.checks[i]);

			if (checkPoint.x < 0 || checkPoint.x >= information.maxX ||
				checkPoint.y < 0 || checkPoint.y >= information.maxY)
				continue;

			Node checkNode;
			checkNode.position = checkPoint;
			checkNode.heuristic = INT_MIN;
			checkNode.steps = checkSteps;

			if (!information.canPlaceNode(&checkNode)) continue;

			PathFinding_addNode(&checkNode);
		}
	}
	return false;
}