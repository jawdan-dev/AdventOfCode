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

GatedNode* findGate(const char from1[3], const char from2[3], const GateType gateType, const char to[3]) {
	for (int i = 0; i < gatedCount; i++) {
		GatedNode* node = &gatedNodes[i];
		if (gateType != NONE && node->gate != gateType) continue;

		bool valid = true;
		for (int j = 0; j < 3 && valid; j++)
			if ((from1[j] != '*' && node->from1->name[j] != from1[j]) ||
				(from2[j] != '*' && node->from2->name[j] != from2[j]) ||
				(to[j] != '*' && node->to->name[j] != to[j]))
				valid = false;
		if (!valid) {
			valid = true;
			for (int j = 0; j < 3 && valid; j++)
				if ((from1[j] != '*' && node->from2->name[j] != from1[j]) ||
					(from2[j] != '*' && node->from1->name[j] != from2[j]) ||
					(to[j] != '*' && node->to->name[j] != to[j]))
					valid = false;
		}
		if (!valid) continue;

		return node;
	}
	return nullptr;
}

#define GATE_ERROR(gate, msg) \
	if (gate == nullptr) { \
		printf(msg, depth); \
		return; \
	}

#define SWAP_MAX 8
Node* swapped[SWAP_MAX];
int swapCount = 0;

void swapOutput(GatedNode* a, GatedNode* b) {
	if (a == nullptr || b == nullptr) return;

	Node* temp = a->to;
	a->to = b->to;
	b->to = temp;

	ASSERT(swapCount < SWAP_MAX, "Too many swaps....");
	swapped[swapCount++] = a->to;
	swapped[swapCount++] = b->to;
}

void validateNodes(const int depth, const char carryNodeName[3]) {
	GatedNode* output = nullptr, * carry = nullptr;

	if (depth == 0) {
		output = findGate("x00", "y00", XOR, "z00");
		carry = findGate("x00", "y00", AND, "***");
	} else {
		char outputName[4];
		snprintf(outputName, ARRAY_SIZE(outputName), "z%.2i", depth);

		output = findGate(carryNodeName, "***", XOR, outputName);
		if (output == nullptr) {
			GatedNode* outputSwap = findGate("***", "***", NONE, outputName);
			if (outputSwap != nullptr) {
				output = findGate(carryNodeName, "***", XOR, "***");
				swapOutput(output, outputSwap);
			}
		}
		GATE_ERROR(output, "Failed to find output gate for %i.");

		Node* midNode = (strncmp(output->from1->name, carryNodeName, 3) == 0)
			? output->from2
			: output->from1;

		char inputXName[4], inputYName[4];
		snprintf(inputXName, ARRAY_SIZE(inputXName), "x%.2i", depth);
		snprintf(inputYName, ARRAY_SIZE(inputYName), "y%.2i", depth);

		GatedNode* midInput = findGate(inputXName, inputYName, XOR, midNode->name);
		if (midInput == nullptr) {
			GatedNode* midSwap = findGate(inputXName, inputYName, AND, midNode->name);
			if (midSwap != nullptr) {
				midInput = findGate(inputXName, inputYName, XOR, "***");
				swapOutput(midInput, midSwap);
			}
		}
		GATE_ERROR(midInput, "Failed to find midInput gate for %i.");

		// NOTE: Sorry if youre using this, but mine worked with whats here.
		GatedNode* inputCarry = findGate(inputXName, inputYName, AND, "***");
		GatedNode* midCarry = findGate(midNode->name, carryNodeName, AND, "***");
		GATE_ERROR(inputCarry, "Failed to find inputCarry gate for %i.");
		GATE_ERROR(midCarry, "Failed to find midCarry gate for %i.");

		carry = findGate(midCarry->to->name, inputCarry->to->name, OR, "***");
	}

	GATE_ERROR(output, "Failed to find output gate for %i.");
	GATE_ERROR(carry, "Failed to find carry gate for %i.");

	if (carry->to->name[0] != 'z') validateNodes(depth + 1, carry->to->name);
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
	validateNodes(0, "***");
	//
	bool sorted = false;
	while (!sorted) {
		sorted = true;
		for (int i = 1; i < swapCount; i++) {
			if (swapped[i - 1]->name[0] < swapped[i]->name[0] ||
			   (swapped[i - 1]->name[0] == swapped[i]->name[0] && swapped[i - 1]->name[1] < swapped[i]->name[1]) ||
			   (swapped[i - 1]->name[0] == swapped[i]->name[0] && swapped[i - 1]->name[1] == swapped[i]->name[1] && swapped[i - 1]->name[2] <= swapped[i]->name[2])) continue;
			sorted = false;

			Node* temp = swapped[i - 1];
			swapped[i - 1] = swapped[i];
			swapped[i] = temp;
		}
	}
	//
	for (int i = 0; i < swapCount; i++) {
		if (i > 0) printf(",");
		printf("%c%c%c", swapped[i]->name[0], swapped[i]->name[1], swapped[i]->name[2]);
	}
}