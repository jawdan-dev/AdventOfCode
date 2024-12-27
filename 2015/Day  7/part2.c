#include <common/common.h>

typedef uint16_t ValueType;

typedef enum {
	NONE,
	SET,
	AND,
	OR,
	LEFTSHIFT,
	RIGHTSHIFT,
	NOT,
} GateType;

const GateType getStringGateType(const char* string) {
	static struct {
		GateType type;
		const char* string;
	} gates[] = {
		{.type = SET, .string = "->" },
		{.type = AND, .string = "AND" },
		{.type = OR, .string = "OR" },
		{.type = LEFTSHIFT, .string = "LSHIFT" },
		{.type = RIGHTSHIFT, .string = "RSHIFT" },
		{.type = NOT, .string = "NOT" },
	};

	for (int i = 0; i < strlen(string); i++) {
		for (int j = 0; j < ARRAY_SIZE(gates); j++) {
			if (i + strlen(gates[j].string) > strlen(string)) continue;

			bool matching = true;
			for (int k = 0; k < strlen(gates[j].string) && matching; k++)
				if (string[i + k] != gates[j].string[k])
					matching = false;
			if (!matching) continue;

			return gates[j].type;
		}
	}

	return NONE;
}

#define NAME_MAX 10

typedef struct {
	char name[NAME_MAX + 1];
	ValueType value;
	bool processed;
} Node;

typedef struct {
	GateType type;
	Node* leftNode;
	ValueType leftValue;
	Node* rightNode;
	ValueType rightValue;
	Node* targetNode;
} Gate;

#define NODES_MAX 1000
Node nodes[NODES_MAX];
int nodeCount = 0;
#define GATES_MAX 1000
Gate gates[GATES_MAX];
int gateCount = 0;

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

		nodes[found].value = 0;
		nodes[found].processed = false;
	}

	return &nodes[found];
}

void processGates() {
	// Process.
	bool fullyProcessed = false;
	while (!fullyProcessed) {
		fullyProcessed = true;
		for (int i = 0; i < gateCount; i++) {
			Gate* const gate = &gates[i];
			if (gate->targetNode->processed) continue;
			if ((gate->leftNode != nullptr && !gate->leftNode->processed) ||
				(gate->rightNode != nullptr && !gate->rightNode->processed)) continue;
			//
			const ValueType 
				left = gate->leftNode != nullptr ? gate->leftNode->value : gate->leftValue,
				right = gate->rightNode != nullptr ? gate->rightNode->value : gate->rightValue;
			//
			switch (gate->type) {
				case SET: gate->targetNode->value = left; break;
				case AND: gate->targetNode->value = left & right;  break;
				case OR: gate->targetNode->value = left | right; break;
				case LEFTSHIFT: gate->targetNode->value = left << right; break;
				case RIGHTSHIFT: gate->targetNode->value = left >> right; break;
				case NOT: gate->targetNode->value = ~left; break;
			}
			//
			gate->targetNode->processed = true;
			fullyProcessed = false;
		}
	}
	
	// Reset.
	for (int i = 0; i < nodeCount; i++) 
		nodes[i].processed = false;
}

int main() {
	FileReader fileReader = FileReader_getInput();
	char left[NAME_MAX + 1], right[NAME_MAX + 1], target[NAME_MAX + 1];
	while (!fileReader.endReached) {
		FileReader_readUntilDelim(&fileReader, "\n");
		//
		const GateType gate = getStringGateType(fileReader.readBuffer);
		if (gate == NONE) continue;
		//
		ASSERT(gateCount < GATES_MAX, "Gate max exceeded.");
		//
		left[0] = '\0';
		right[0] = '\0';
		target[0] = '\0';
		switch (gate) {
			case SET: sscanf(fileReader.readBuffer, "%s -> %s", left, target); break;
			case AND: sscanf(fileReader.readBuffer, "%s AND %s -> %s", left, right, target); break;
			case OR: sscanf(fileReader.readBuffer, "%s OR %s -> %s", left, right, target); break;
			case LEFTSHIFT: sscanf(fileReader.readBuffer, "%s LSHIFT %s -> %s", left, right, target); break;
			case RIGHTSHIFT: sscanf(fileReader.readBuffer, "%s RSHIFT %s -> %s", left, right, target); break;
			case NOT: sscanf(fileReader.readBuffer, "NOT %s -> %s", left, target); break;
		}
		//
		gates[gateCount].leftNode = nullptr;
		gates[gateCount].leftValue = 0;
		gates[gateCount].rightNode = nullptr;
		gates[gateCount].rightValue = 0;
		gates[gateCount].targetNode = nullptr;
		//
		if ('0' <= left[0] && left[0] <= '9') sscanf(left, "%i", &gates[gateCount].leftValue);
		else if (left[0] != '\0') gates[gateCount].leftNode = getNode(left);
		//
		if ('0' <= right[0] && right[0] <= '9') sscanf(right, "%i", &gates[gateCount].rightValue);
		else if (right[0] != '\0') gates[gateCount].rightNode = getNode(right);
		//
		ASSERT(target[0] != '\0', "Failed to read target node.");
		gates[gateCount].targetNode = getNode(target);
		//
		gates[gateCount].type = gate;
		gateCount++;
	}
	//
	processGates();
	//
	Node* b = getNode("b");
	b->value = getNode("a")->value;
	b->processed = true;
	//
	processGates();
	printf("a's signal: %u\n", getNode("a")->value);
}