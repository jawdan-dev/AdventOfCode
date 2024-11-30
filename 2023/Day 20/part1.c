// Load FileBuffer.
#define FILEBUFFER_READBUFFERSIZE 100
enum ReadState {
	ReadState_Line,
};
#include "../Common/FileBuffer.h"

#define NODE_MAX 200
#define LABEL_MAX 20
#define TARGET_MAX 20
#define INPUT_MAX 20

typedef enum NodeType {
	NodeType_Broadcaster,
	NodeType_TFlipFlop,
	NodeType_Conjuction,
} NodeType;

typedef struct Node {
	char label[LABEL_MAX];
	char targetLabels[LABEL_MAX][TARGET_MAX];
	size_t targetIndices[TARGET_MAX];
	size_t targetCount;

	NodeType nodeType;
	bool tFlipFlopped;

	bool inputStates[INPUT_MAX];
	size_t inputCount;
} Node;

const size_t findNode(
	Node* const nodes, const size_t nodeCount,
	const char* targetLabel
);
void handleSignal(
	Node* const nodes, const size_t nodeCount,
	const size_t targetNode,
	const bool signalHigh, const size_t signalIndex,
	size_t* const lowPulseCount, size_t* const highPulseCount
);

// Entry point.
int main() {
	// Create file buffer.
	struct FileBuffer fileBuffer;
	FileBuffer_OpenFile(&fileBuffer, "input.txt");

	// File delims.
	struct FileDelim delims[] = {
		{ '\n', ReadState_Line }
	};
	FileBuffer_SetDelims(&fileBuffer, delims, sizeof(delims) / sizeof(*delims), ReadState_Line);

	// Interpretation variables.
	Node nodes[NODE_MAX];
	size_t nodeCount = 0;

	// Interpret file.
	while (FileBuffer_ReadUntilNextDelim(&fileBuffer)) {
		// Processing.
		switch (fileBuffer.readState) {
			case ReadState_Line: {
				// Check node max.
				if (nodeCount >= NODE_MAX) {
					fprintf(stderr, "Node max exceeded.\n");
					abort();
				}

				// Get node type.
				char nodeType = fileBuffer.readBuffer[0];
				size_t labelOffset = (nodeType == 'b' ? 0 : 1);

				// Find next space.
				size_t labelSize = 0;
				while (fileBuffer.readBuffer[labelSize + labelOffset] != ' ') labelSize++;

				// Check if label is too long.
				if (labelSize >= LABEL_MAX) {
					fprintf(stderr, "Label max exceeded.\n");
					abort();
				}

				// Read label.
				char label[LABEL_MAX];
				strncpy(label, fileBuffer.readBuffer + labelOffset, labelSize);
				label[labelSize] = '\0';

				// Get target node.
				Node* const currentNode = &nodes[nodeCount];
				nodeCount++;

				// Initialize node.
				strcpy(currentNode->label, label);
				currentNode->tFlipFlopped = false;
				for (size_t i = 0; i < INPUT_MAX; i++)
					currentNode->inputStates[i] = false;
				currentNode->inputCount = 0;

				// Handle based on type.
				switch (nodeType) {
					case 'b': currentNode->nodeType = NodeType_Broadcaster; break;
					case '%': currentNode->nodeType = NodeType_TFlipFlop; break;
					case '&': currentNode->nodeType = NodeType_Conjuction; break;
				}

				// Read targets.
				currentNode->targetCount = 0;
				char* targetStr = strtok(fileBuffer.readBuffer + labelSize + 4, ", ");
				while (targetStr != nullptr) {
					if (strlen(targetStr) >= LABEL_MAX) {
						fprintf(stderr, "Target label max exceeded.\n");
						abort();
					}

					// Read data.
					strncpy(currentNode->targetLabels[currentNode->targetCount], targetStr, LABEL_MAX);
					currentNode->targetCount++;

					// Increment.
					targetStr = strtok(nullptr, ", ");
				}
			} break;
		}
	}

	// Counters.
	size_t lowPulses, highPulses;

	for (size_t i = 0; i < nodeCount; i++) {
		Node* const currentNode = &nodes[i];
		printf("%s -> ", currentNode->label);
		for (size_t j = 0; j < currentNode->targetCount; j++) {
			printf("%s, ", currentNode->targetLabels[j]);
		}
		printf("\n");
	}

	// Find broadcaster.
	const size_t broadcaster = findNode(nodes, nodeCount, "broadcaster");

	for (size_t i = 0; i < nodeCount; i++) {
		Node* const currentNode = &nodes[i];
		for (size_t j = 0; j < currentNode->targetCount; j++) {
			const size_t targetIndex = findNode(nodes, nodeCount, currentNode->targetLabels[j]);
			if (targetIndex >= nodeCount) {
				continue;
			}
			Node* const targetNode = &nodes[targetIndex];
			if (targetNode->nodeType == NodeType_Conjuction) {
				// Setup index.
				currentNode->targetIndices[j] = targetNode->inputCount;
				targetNode->inputCount++;
			} else {
				currentNode->targetIndices[j] = NODE_MAX;
			}
		}
	}

	// Send pulses.
	for (size_t i = 0; i < 1000; i++) {
		handleSignal(
			nodes, nodeCount,
			broadcaster,
			false, SIZE_MAX,
			&lowPulses, &highPulses
		);
	}

	// Output answer.
	printf("Low pulses: %zu\n", lowPulses);
	printf("High pulses: %zu\n", highPulses);
	printf("Output: %zu\n", lowPulses * highPulses);
	return 0;
}

const size_t findNode(
	Node* const nodes, const size_t nodeCount,
	const char* targetLabel
) {
	// Find node.
	for (size_t i = 0; i < nodeCount; i++)
		if (strcmp(nodes[i].label, targetLabel) == 0)
			return i;

	// Node not found.
	return SIZE_MAX;
}
void handleSignal(
	Node* const nodes, const size_t nodeCount,
	const size_t targetNode,
	const bool signalHigh, const size_t signalIndex,
	size_t* const lowPulseCount, size_t* const highPulseCount
) {
	// Account for pulses.
	if (signalHigh) (*highPulseCount)++;
	else (*lowPulseCount)++;

	// Error check.
	if (targetNode >= nodeCount) return;

	// Get node.
	Node* const currentNode = &nodes[targetNode];

	// Handle based on type.
	switch (currentNode->nodeType) {
		case NodeType_Broadcaster: {
			// Broad cast to all.
			for (size_t i = 0; i < currentNode->targetCount; i++) {
				handleSignal(
					nodes, nodeCount,
					findNode(nodes, nodeCount, currentNode->targetLabels[i]),
					signalHigh, currentNode->targetIndices[i],
					lowPulseCount, highPulseCount
				);
			}
		} break;
		case NodeType_TFlipFlop: {
			// Ignore high signals.
			if (signalHigh) break;

			// Flip state.
			currentNode->tFlipFlopped = !currentNode->tFlipFlopped;
			const bool activeHigh = currentNode->tFlipFlopped;

			// Send signal to targets.
			for (size_t i = 0; i < currentNode->targetCount; i++) {
				handleSignal(
					nodes, nodeCount,
					findNode(nodes, nodeCount, currentNode->targetLabels[i]),
					activeHigh, currentNode->targetIndices[i],
					lowPulseCount, highPulseCount
				);
			}
		} break;
		case NodeType_Conjuction: {
			if (signalIndex >= currentNode->inputCount) {
				fprintf(stderr, "Signal index exceeded input count. %zu vs %zu", signalIndex, currentNode->inputCount);
				abort();
			}
			currentNode->inputStates[signalIndex] = signalHigh;

			// Get if all input states are active.
			bool allActive = true;
			for (size_t i = 0; i < currentNode->inputCount; i++) {
				if (!currentNode->inputStates[i]) {
					allActive = false;
					break;
				}
			}

			// Send signal to targets.
			for (size_t i = 0; i < currentNode->targetCount; i++) {
				handleSignal(
					nodes, nodeCount,
					findNode(nodes, nodeCount, currentNode->targetLabels[i]),
					!allActive, currentNode->targetIndices[i],
					lowPulseCount, highPulseCount
				);
			}
		} break;
	}
}
