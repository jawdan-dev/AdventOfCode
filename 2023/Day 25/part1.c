// Load FileBuffer.
#define FILEBUFFER_READBUFFERSIZE 100
enum ReadState {
	ReadState_Line,
};
#include "../Common/FileBuffer.h"

#define COMPONENT_MAX 2000
#define LABEL_MAX 20
#define CONNECTIONS_MAX 100

#define CONNECTION_SEARCH 3

typedef struct Component {
	char label[LABEL_MAX];
	struct Component* connections[CONNECTIONS_MAX];
	size_t connectionCount;
	size_t index;
} Component;

Component* const insertComponent(Component* const components, size_t* const componentCount, const char* const label);
const size_t findComponent(const Component* const components, const size_t componentCount, const char* const label);

const bool disconnectWires(
	Component* const components, const size_t componentCount,
	Component** const sortedComponents,
	const size_t disconnectCount,
	const size_t maxComponent, const size_t maxConnection
);
const size_t getGroupCount(const Component* const components, const size_t componentCount);
void setGroupRec(const Component* const component, size_t* const groupData, const size_t groupID);

void mapComponentDepth(
	Component* const components, const size_t componentCount,
	size_t* const componentFrequency
);
void mapComponentDistances(const Component* const component, size_t* const distanceData, const size_t distance);
void totalPath(const Component* const target, const size_t* const distanceData, size_t* const componentFrequency);

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
	Component* components = malloc(COMPONENT_MAX * sizeof(Component));
	size_t componentCount = 0;

	// Interpret file.
	while (FileBuffer_ReadUntilNextDelim(&fileBuffer)) {
		// Processing.
		switch (fileBuffer.readState) {
			case ReadState_Line: {
				// Get current label.
				char* const label = strtok(fileBuffer.readBuffer, ":");
				Component* const wire = insertComponent(components, &componentCount, label);

				// Read values.
				char* connectionStr = strtok(fileBuffer.readBuffer + strlen(label) + 1, " ");
				while (connectionStr != nullptr) {
					// Read data.
					Component* const connection = insertComponent(components, &componentCount, connectionStr);

					// Error check.
					if (wire->connectionCount >= CONNECTIONS_MAX ||
						connection->connectionCount >= CONNECTIONS_MAX) {
						fprintf(stderr, "Connection max exceeded.\n");
						abort();
					}

					// Add connection.
					wire->connections[wire->connectionCount++] = connection;
					connection->connections[connection->connectionCount++] = wire;

					// Increment.
					connectionStr = strtok(nullptr, " ");
				}
			} break;
		}
	}

	// Debug output.
	printf("%zu connections found.\n", componentCount);

	size_t componentFrequency[COMPONENT_MAX];
	for (size_t i = 0; i < componentCount; i++) componentFrequency[i] = 0;
	printf("Mapping component depths.\n");
	mapComponentDepth(components, componentCount, componentFrequency);

	// Sort to find lowest first.
	Component* lowest[COMPONENT_MAX];
	for (size_t i = 0; i < componentCount; i++) lowest[i] = &components[i];
	for (size_t i = 0; i < componentCount; i++) {
		for (size_t j = 1; j < componentCount - i; j++) {
			if (componentFrequency[lowest[j]->index] > componentFrequency[lowest[j - 1]->index]) {
				Component* const temp = lowest[j];
				lowest[j] = lowest[j - 1];
				lowest[j - 1] = temp;
			}
		}
	}

	// Debug output.
	printf("Best found:\n");
	for (size_t i = 0; i < 25; i++) {
		printf("- %s: %zu\n", lowest[i]->label, componentFrequency[lowest[i]->index]);
	}

	// Disconnect wires.
	printf("Finding disconnect-ables.\n");
	disconnectWires(components, componentCount, lowest, CONNECTION_SEARCH, SIZE_MAX, SIZE_MAX);

	// Cleanup.
	free(components);
	return 0;
}

Component* const insertComponent(Component* const components, size_t* const componentCount, const char* const label) {
	// Check if already added.
	const size_t foundIndex = findComponent(components, *componentCount, label);
	if (foundIndex != SIZE_MAX) return &components[foundIndex];

	// Error check.
	if (*componentCount >= COMPONENT_MAX) {
		fprintf(stderr, "Component max exceeded.\n");
		abort();
	}
	if (strlen(label) + 1 >= LABEL_MAX) {
		fprintf(stderr, "Label max exceeded.\n");
		abort();
	}

	// Insert wire.
	const size_t index = *componentCount;
	strcpy(components[index].label, label);
	components[index].connectionCount = 0;
	components[index].index = index;

	// Update details.
	(*componentCount)++;

	// Return.
	return &components[index];
}
const size_t findComponent(const Component* const components, const size_t componentCount, const char* const label) {
	// Find wire with matching label.
	for (size_t i = 0; i < componentCount; i++)
		if (strcmp(components[i].label, label) == 0)
			return i;
	// Not found.
	return SIZE_MAX;
}

const bool disconnectWires(
	Component* const components, const size_t componentCount,
	Component** const sortedComponents,
	const size_t disconnectCount,
	const size_t maxComponent, const size_t maxConnection
) {
	if (disconnectCount <= 0) {
		// Get group count.
		if (getGroupCount(components, componentCount) == 2) {
			printf("Group found after removing:\n");
			return true;
		}
		return false;
	}

	for (size_t i = 0; i < componentCount && i <= maxComponent; i++) {
		Component* const component = sortedComponents[i];
		const size_t maxConnectionLocal = i == maxComponent ? maxConnection : SIZE_MAX;
		for (size_t j = 0; j < component->connectionCount && j < maxConnectionLocal; j++) {
			Component* const target = component->connections[j];

			// Swap base.
			component->connections[j] = component->connections[component->connectionCount - 1];
			component->connections[component->connectionCount - 1] = target;
			component->connectionCount--;

			// Find target connection index.
			size_t targetIndex = 0;
			while (targetIndex < target->connectionCount && target->connections[targetIndex] != component) targetIndex++;

			// Swap target.
			target->connections[targetIndex] = target->connections[target->connectionCount - 1];
			target->connections[target->connectionCount - 1] = component;
			target->connectionCount--;

			// Recurse.
			if (disconnectWires(components, componentCount, sortedComponents, disconnectCount - 1, i, j)) {
				printf("- %s -> %s\n", component->label, target->label);
				return true;
			}

			// Reset connection counts.
			component->connectionCount++;
			target->connectionCount++;
		}
		if (disconnectCount == CONNECTION_SEARCH) printf("Processed: %zu\t\t\t\r", i);
	}

	return false;
}
const size_t getGroupCount(const Component* const components, const size_t componentCount) {
	size_t componentGroups[COMPONENT_MAX];
	for (size_t i = 0; i < componentCount; i++) componentGroups[i] = SIZE_MAX;

	size_t activeGroupID = 0;
	for (size_t i = 0; i < componentCount; i++) {
		const Component* const component = &components[i];

		// Ignore if already in group.
		if (componentGroups[i] != SIZE_MAX) continue;

		// Set group.
		setGroupRec(component, componentGroups, activeGroupID++);
	}

	if (activeGroupID == 2) {
		size_t groupCounts[2];
		groupCounts[0] = 0;
		groupCounts[1] = 0;

		// Total groups.
		for (size_t i = 0; i < componentCount; i++)
			groupCounts[componentGroups[i]]++;

		// Output total.
		printf("Groups of %zu and %zu => %zu\n", groupCounts[0], groupCounts[1], groupCounts[0] * groupCounts[1]);
	}

	// Return group count.
	return activeGroupID;
}
void setGroupRec(const Component* const component, size_t* groupData, const size_t groupID) {
	if (groupData[component->index] != SIZE_MAX) return;

	// Set group.
	groupData[component->index] = groupID;

	// Recurse connections.
	for (size_t i = 0; i < component->connectionCount; i++) {
		setGroupRec(component->connections[i], groupData, groupID);
	}
}

void mapComponentDepth(
	Component* const components, const size_t componentCount,
	size_t* const componentFrequency
) {
	size_t distanceMap[COMPONENT_MAX];
	// Find connection point.
	for (size_t i = 0; i < componentCount; i++) {
		Component* const component = &components[i];

		// Clear depth.
		for (size_t k = 0; k < componentCount; k++)
			distanceMap[k] = SIZE_MAX;

		// Create depth map.
		mapComponentDistances(component, distanceMap, 0);

		for (size_t j = i + 1; j < componentCount; j++) {
			// Disable connection.
			Component* const target = &components[j];

			// Total the path.
			totalPath(target, distanceMap, componentFrequency);
		}
		printf("Processed: %zu\t\t\t\r", i);
	}
}
void mapComponentDistances(const Component* const component, size_t* distanceData, const size_t distance) {
	if (distanceData[component->index] <= distance) return;

	// Set group.
	distanceData[component->index] = distance;

	// Set max distances.
	const size_t distanceNextNext = distance + 2;
	for (size_t i = 0; i < component->connectionCount; i++) {
		if (distanceData[component->connections[i]->index] > distanceNextNext)
			distanceData[component->connections[i]->index] = distanceNextNext;
	}

	// Recurse connections.
	const size_t distanceNext = distance + 1;
	for (size_t i = 0; i < component->connectionCount; i++) {
		mapComponentDistances(component->connections[i], distanceData, distanceNext);
	}
}
void totalPath(const Component* const target, const size_t* const distanceData, size_t* const componentFrequency) {
	// Increment count.
	componentFrequency[target->index]++;

	// Ignore base case.
	if (distanceData[target->index] == 0) return;

	// Get next node.
	const size_t distance = distanceData[target->index];
	const size_t distanceNext = distance - 1;
	for (size_t i = 0; i < target->connectionCount; i++) {
		if (distanceData[target->connections[i]->index] > distanceNext) continue;

		totalPath(target->connections[i], distanceData, componentFrequency);
		break;
	}
}