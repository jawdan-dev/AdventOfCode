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

typedef struct {
	Node* root;
	bool dacVisited, fftVisited;
}MemoizationKey;
typedef uint64_t MemoizationResult;
#include <common/memoization.h>

Node* target;
Node* dac;
Node* fft;
const uint64_t getPathCounts(Node* root) {
	if (root->visited)
		return 0;

	if (root == target) {
		if (dac->visited && fft->visited)
			return 1;
		return 0;
	}

	const MemoizationKey key = {
		.root = root,
		.dacVisited = dac->visited,
		.fftVisited = fft->visited,
	};
	MemoizationResult result = 0;
	if (Memoization_getResult(&key, &result))
		return result;

	root->visited = true;

	uint64_t pathCount = 0;
	for (size_t i = 0; i < root->connectionCount; i++) {
		pathCount += getPathCounts(root->connections[i]);
	}

	root->visited = false;
	Memoization_add(&key, &pathCount);
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
	target = getNode((Name){'o', 'u', 't'});
	dac = getNode((Name){'d', 'a', 'c'});
	fft = getNode((Name){'f', 'f', 't'});
	printf("Total paths: %lli\n", getPathCounts(getNode((Name){'s', 'v', 'r'})));
}