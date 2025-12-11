#include <common/common.h>

#define NODE_MAX 636

typedef struct {
	char data[3];
} Name;
typedef struct {
	Name name;
	void** connections;
	size_t connectionCount;
	bool visited;
} Node;
Node nodes[NODE_MAX];
size_t nodeCount = 0;

Node* getNode(Name name) {
	for (size_t i = 0; i < nodeCount; i++) {
		if (memcmp(nodes[i].name.data, name.data, sizeof(name)) == 0)
			return &nodes[i];
	}

	ASSERT(nodeCount < NODE_MAX, "Increase NODE_MAX");
	Node* node = &nodes[nodeCount++];
	node->name = name;
	node->connections = nullptr;
	node->connectionCount = 0;
	node->visited = false;
	return node;
}
void addConnection(Name from, Name to) {
	Node *fromNode = getNode(from),
		 *toNode = getNode(to);

	fromNode->connections = realloc(fromNode->connections, (fromNode->connectionCount + 1) * sizeof(Node));
	((Node**)fromNode->connections)[fromNode->connectionCount] = toNode;
	fromNode->connectionCount++;
}

Node* target;
const uint64_t getPathCounts(Node* root) {
	if (root == target) return 1;

	root->visited = true;

	uint64_t pathCount = 0;
	for (size_t i = 0; i < root->connectionCount; i++) {
		pathCount += getPathCounts(root->connections[i]);
	}

	root->visited = false;
	return pathCount;
}

int main() {
	FileReader fileReader = FileReader_getInput();

	while (!fileReader.endReached) {
		FileReader_readUntilDelim(&fileReader, ":");
		const Name rootName;
		memcpy(rootName.data, fileReader.readBuffer, sizeof(rootName));

		FileReader_readUntilDelim(&fileReader, "\n");
		const size_t count = (fileReader.readBufferLength + 1) / 4;
		char* buffer = fileReader.readBuffer + 1;
		for (int n = 0; n < count; n++) {
			const Name name = {.data = {buffer[(n * 4)],
										buffer[(n * 4) + 1],
										buffer[(n * 4) + 2]}};
			addConnection(rootName, name);
		}
	}
	target = getNode((Name){ 'o', 'u', 't' });
	printf("Total paths: %lli\n", getPathCounts(getNode((Name){ 'y', 'o', 'u' })));
}