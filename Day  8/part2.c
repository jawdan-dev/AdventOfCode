// Load FileBuffer.
#define FILEBUFFER_READBUFFERSIZE 1000
enum ReadState {
	ReadState_Move,
	ReadState_NodeLabel,
	ReadState_NodeChildren,
};
#include "../Common/FileBuffer.h"

enum Move {
	Move_Left,
	Move_Right,
};

struct Node {
	char label[3];
	struct Node* left, * right;
};
struct Node* const Node_findNode(struct Node* const nodeList, size_t* const nodeListLength, const char label[3]);

// Entry point.
int main() {
	// Create file buffer.
	struct FileBuffer fileBuffer;
	FileBuffer_OpenFile(&fileBuffer, "input.txt");

	// File delims.
	struct FileDelim delims[] = {
		{ '\n', ReadState_Move },
		{ '(', ReadState_NodeLabel },
		{ ')', ReadState_NodeChildren },
	};
	FileBuffer_SetDelims(&fileBuffer, delims, sizeof(delims) / sizeof(*delims), ReadState_Move);

	// Interpretation variables.
	enum Move moveList[500];
	size_t moveCount = 0;
	struct Node nodeList[1000];
	size_t nodeCount = 0;
	struct Node* targetNode = nullptr;
	struct Node* startingNodes[200];
	size_t startingNodeCount = 0;

	// Interpret file.
	while (FileBuffer_ReadUntilNextDelim(&fileBuffer)) {
		// Processing.
		switch (fileBuffer.readState) {
			case ReadState_Move: {
				for (size_t i = 0; i < strlen(fileBuffer.readBuffer); i++) {
					// Error check.
					if (moveCount >= sizeof(moveList) / sizeof(*moveList)) {
						fprintf(stderr, "Not enough space allocated for move list.\n");
						abort();
					}

					// Add move.
					switch (fileBuffer.readBuffer[i]) {
						default: continue;
						case 'L': moveList[moveCount] = Move_Left; break;
						case 'R': moveList[moveCount] = Move_Right; break;
					}
					moveCount++;
				}
			} break;
			case ReadState_NodeLabel: {
				// Get target node.
				targetNode = Node_findNode(nodeList, &nodeCount, fileBuffer.readBuffer);

				// Check if a starting node.
				if (fileBuffer.readBuffer[2] == 'A') {
					// Error check.
					if (startingNodeCount >= sizeof(startingNodes) / sizeof(*startingNodes)) {
						fprintf(stderr, "Not enough space allocated for starting nodes.\n");
						abort();
					}
					// Add starting node.
					startingNodes[startingNodeCount] = targetNode;
					startingNodeCount++;
				}

			} break;
			case ReadState_NodeChildren: {
				if (targetNode == nullptr) break;

				// Find left label.
				char* label = strtok(fileBuffer.readBuffer, " ");
				targetNode->left = Node_findNode(nodeList, &nodeCount, label);

				// Find right label.
				label = strtok(nullptr, " ");
				targetNode->right = Node_findNode(nodeList, &nodeCount, label);
			} break;
		}
	}

	size_t bestNodesOnZ = 0;
	size_t totalMoves = 0, currentMove;
	if (moveCount > 0 && nodeCount > 0 && startingNodeCount > 0) {
		// Find all start nodes' traversal counts.
		size_t startingNodeMoves[sizeof(startingNodes) / sizeof(*startingNodes)];

		for (size_t n = 0; n < startingNodeCount; n++) {
			// Initialize variables.
			startingNodeMoves[n] = 0;
			currentMove = 0;

			// Traverse.
			while (startingNodes[n]->label[2] != 'Z') {
				// Move.
				switch (moveList[currentMove]) {
					case Move_Left: startingNodes[n] = startingNodes[n]->left; break;
					case Move_Right: startingNodes[n] = startingNodes[n]->right; break;
				}

				// Error check.
				if (startingNodes[n] == nullptr) {
					fprintf(stderr, "Invalid node has been traversed to.\n");
					abort();
				}

				// Increment details.
				startingNodeMoves[n]++;
				currentMove = (currentMove + 1) % moveCount;
			}
		}

		// Calculate LCM.
		totalMoves = startingNodeMoves[0];
		for (size_t i = 1; i < startingNodeCount; i++) {
			// LCM.
			totalMoves = LCM(totalMoves, startingNodeMoves[i]);
		}
	}

	// Output answer.
	printf("Output: %zu\n", totalMoves);
	return 0;
}

struct Node* const Node_findNode(struct Node* const nodeList, size_t* const nodeListLength, const char label[3]) {
	// Find if in list.
	for (size_t i = 0; i < *nodeListLength; i++)
		if (strncmp(nodeList[i].label, label, 3) == 0)
			return &nodeList[i];

	// Otherwise, add to list.
	struct Node* const node = &nodeList[*nodeListLength];
	// Update details.
	(*nodeListLength)++;

	// Initialize.
	strncpy(node->label, label, 3);
	node->left = nullptr;
	node->right = nullptr;

	return node;

}