#include <common/common.h>

typedef struct {
	char name[3];
	bool state;
	bool processed;
} Node;

typedef enum {
	NONE,
	AND,
	OR,
	XOR,
} GateType;

typedef struct {
	Node* from1;
	Node* from2;
	GateType gate;
	Node* to;
} GatedNode;

#define NODE_MAX 1000
Node nodes[NODE_MAX];
int nodeCount = 0;
#define GATED_MAX 1000
GatedNode gatedNodes[GATED_MAX];
int gatedCount = 0;

Node* getNode(const char name[3]) {
	int found = -1;
	for (int i = 0; i < nodeCount && found == -1; i++)
		if (nodes[i].name[0] == name[0] && nodes[i].name[1] == name[1] && nodes[i].name[2] == name[2])
			found = i;

	if (found == -1) {
		found = nodeCount;
		nodes[found] = (Node) {
			.state = false,
			.processed = false,
		};
		strncpy(nodes[found].name, name, 3);
		nodeCount++;
	}

	return &nodes[found];
}

const GateType getGate(const char gate[4]) {
	if (strncmp(gate, "AND", 3) == 0) return AND;
	if (strncmp(gate, "XOR", 3) == 0) return XOR;
	if (strncmp(gate, "OR", 2) == 0) return OR;
	return NONE;
}

int main() {
	FileReader fileReader = FileReader_getInput();
	//
	while (!fileReader.endReached) {
		FileReader_readUntilDelim(&fileReader, "\n");
		if (fileReader.readBufferLength <= 0) break;
		//
		char name[3];
		int state;
		sscanf(fileReader.readBuffer, "%c%c%c: %i", &name[0], &name[1], &name[2], &state);
		//
		nodes[nodeCount] = (Node) {
			.state = state == 1,
			.processed = true,
		};
		strncpy(nodes[nodeCount].name, name, 3);
		nodeCount++;
	}
	//
	while (!fileReader.endReached) {
		FileReader_readUntilDelim(&fileReader, "\n");
		//
		char from1Name[3], from2Name[3], toName[3];
		char gate[4];
		int state;
		sscanf(fileReader.readBuffer, "%c%c%c %s %c%c%c -> %c%c%c", &from1Name[0], &from1Name[1], &from1Name[2], gate, &from2Name[0], &from2Name[1], &from2Name[2], &toName[0], &toName[1], &toName[2]);

		gatedNodes[gatedCount++] = (GatedNode) {
			.from1 = getNode(from1Name),
			.from2 = getNode(from2Name),
			.gate = getGate(gate),
			.to = getNode(toName),
		};
	}
	//
	bool processed = false;
	while (!processed) {
		processed = true;
		for (int i = 0; i < gatedCount; i++) {
			GatedNode *node = &gatedNodes[i];
			if (node->to->processed) continue;
			if (!node->from1->processed || !node->from2->processed) continue;

			bool state = false;
			switch (node->gate) {
				case AND: state = node->from1->state && node->from2->state; break;
				case XOR: state = node->from1->state != node->from2->state; break;
				case OR: state = node->from1->state || node->from2->state; break;
				case NONE: ASSERT(false, "Gate is == NONE"); break;
			}

			node->to->state = state;
			node->to->processed = true;

			processed = false;
		}
	}
	//
	unsigned long long result = 0;
	for (int i = 0; i < nodeCount; i++) {
		if (!nodes[i].processed) {
			printf("Node %i not processed.\n", i);
			continue;
		}
		if (nodes[i].name[0] != 'z') continue;
		if (!nodes[i].state) continue;
		//
		const unsigned long long bit = ((nodes[i].name[1] - '0') * 10) + (nodes[i].name[2] - '0');
		printf("%c%c%c = %c (bit: %llu)\n", nodes[i].name[0], nodes[i].name[1], nodes[i].name[2], nodes[i].state ? 'T' : 'F', bit);
		result |= 0b1llu << bit;
	}
	//
	printf("Result: %llu\n", result);
	for (int i = 64 - 1; i >= 0; i--)
		printf("%i", (result >> i) & 0b1);
}