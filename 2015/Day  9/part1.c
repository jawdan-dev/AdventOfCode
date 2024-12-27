#include <common/common.h>

#define NAME_MAX 16
#define NODE_CONNECTIONS_MAX 20

typedef struct {
	char name[NAME_MAX + 1];
	struct Connection* connections[NODE_CONNECTIONS_MAX];
	int connectionCount;
	bool visited;
} Node;

typedef struct Connection {
	SafeType distance;
	Node* from;
	Node* to;
} Connection;

#define NODES_MAX 1000
Node nodes[NODES_MAX];
int nodeCount = 0;
#define CONNECTIONS_MAX 1000
Connection connections[CONNECTIONS_MAX];
int connectionCount = 0;

Node* const getNode(const char* name) {
	int found = -1;
	for (int i = 0; i < nodeCount && found == -1; i++)
		if (strcmp(nodes[i].name, name) == 0)
			found = i;

	if (found == -1) {
		ASSERT(nodeCount < NODES_MAX, "Node max exceeded.");

		found = nodeCount;
		nodeCount++;

		for (int i = 0; i < strlen(name); i++) nodes[found].name[i] = name[i];
		for (int i = strlen(name); i <= NAME_MAX; i++) nodes[found].name[i] = '\0';

		nodes[found].connectionCount = 0;
		nodes[found].visited = false;
	}

	return &nodes[found];
}

void addConnection(const char* fromName, const char* toName, const int distance) {
	Node* from = getNode(fromName), * to = getNode(toName);
	//
	connections[connectionCount].from = from;
	connections[connectionCount].to = to;
	connections[connectionCount].distance = distance;
	//
	from->connections[from->connectionCount++] = &connections[connectionCount];
	to->connections[to->connectionCount++] = &connections[connectionCount];
	//
	connectionCount++;
}

const SafeType findShortestPath(Node* node, const int depth) {
	SafeType bestDistance = -1;

	node->visited = true;
	for (int i = 0; i < node->connectionCount; i++) {
		Node* next = node->connections[i]->from == node 
			? node->connections[i]->to 
			: node->connections[i]->from;
		
		if (next->visited) continue;

		const SafeType nextShortest = findShortestPath(next, depth + 1);
		if (nextShortest == -1) continue;

		const SafeType distance = 
			node->connections[i]->distance + 
			nextShortest;

		if (bestDistance == -1 || bestDistance > distance) 
			bestDistance = distance;
	}

	node->visited = false;
	return (depth == nodeCount) ? 0 : bestDistance;
}

int main() {
	FileReader fileReader = FileReader_getInput();
	//
	char from[NAME_MAX + 1], to[NAME_MAX + 1];
	int distance;
	while (!fileReader.endReached) {
		FileReader_readUntilDelim(&fileReader, "\n");
		//
		sscanf(fileReader.readBuffer, "%s to %s = %i", from, to, &distance);
		addConnection(from, to, distance);
	}
	//
	SafeType minDistance = -1;
	for (int i = 0; i < nodeCount; i++) {
		const SafeType distance = findShortestPath(&nodes[i], 1);
		if (minDistance == -1 || minDistance > distance) 
			minDistance = distance;
	}
	//
	printf("Minimum distance: %lli\n", minDistance);
}