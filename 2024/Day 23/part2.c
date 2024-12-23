#include <common/common.h>

typedef long long ValueType;

#define CONNECTIONS_MAX 200
#define COMPUTERS_MAX 4000

typedef struct Computer {
	char name[2];
	struct Computer* connections[CONNECTIONS_MAX];
	int connectionCount;
	int traversed;
} Computer;

Computer computers[COMPUTERS_MAX];
int computerCount = 0;

const int addComputer(const char name[2]) {
	int found = -1;
	for (int i = 0; i < computerCount && found == -1; i++)
		if (computers[i].name[0] == name[0] && computers[i].name[1] == name[1])
			found = i;
	if (found == -1) {
		found = computerCount++;
		computers[found] = (Computer) {
			.name = { name[0], name[1] },
			.connectionCount = 0,
			.traversed = false,
		};
	}
	return found;
}

void Computer_addConnection(Computer* this, const char name[2]) {
	int found = -1;
	for (int i = 0; i < this->connectionCount && found == -1; i++)
		if (this->connections[i]->name[0] == name[0] && this->connections[i]->name[1] == name[1])
			found = i;
	if (found != -1) return;

	this->connections[this->connectionCount++] = &computers[addComputer(name)];
}

void addConnection(const char a[2], const char b[2]) {
	const int aIndex = addComputer(a), bIndex = addComputer(b);
	Computer_addConnection(&computers[aIndex], b);
	Computer_addConnection(&computers[bIndex], a);
}

Computer* traversalStack[1000];
int traversalStackSize = 0;

Computer* maxStack[1000];
int maxStackCount = 0;

void getMaxCircularConnections(Computer* this, Computer** remainingConnections, const int remainingConnectionCount) {
	// Early ignore.
	if (remainingConnectionCount <= maxStackCount) return;
	if (this->traversed >= remainingConnectionCount) return;
	this->traversed = remainingConnectionCount;

	if (remainingConnectionCount <= traversalStackSize + 1) {
		if (remainingConnectionCount > maxStackCount) {
			maxStackCount = remainingConnectionCount;
			memcpy(maxStack, remainingConnections, remainingConnectionCount * sizeof(Computer*));
		}
		return;
	}
	// Ignore if already traversed.
	for (int i = 0; i < traversalStackSize; i++)
		if (traversalStack[i] == this)
			return;
	traversalStack[traversalStackSize++] = this;

	Computer* commonConnections[CONNECTIONS_MAX];
	int commonConnectionCount = 0;
	//
	commonConnections[commonConnectionCount++] = this;
	for (int i = 0; i < remainingConnectionCount; i++) {
		for (int j = 0; j < this->connectionCount; j++) {
			if (remainingConnections[i] == this->connections[j]) {
				commonConnections[commonConnectionCount++] = remainingConnections[i];
				break;
			}
		}
	}

	for (int i = 0; i < this->connectionCount; i++)
		getMaxCircularConnections(this->connections[i], commonConnections, commonConnectionCount);

	traversalStackSize--;
}

int main() {
	FileReader fileReader = FileReader_getInput();
	//
	while (!fileReader.endReached) {
		FileReader_readUntilDelim(&fileReader, "\n");
		//
		const char
			first[2] = { fileReader.readBuffer[0], fileReader.readBuffer[1] },
			second[2] = { fileReader.readBuffer[3], fileReader.readBuffer[4] };
		addConnection(first, second);
	}
	//
	for (int i = 0; i < computerCount; i++) {
		printf("%i / %i\r", i + 1, computerCount);
		for (int j = 0; j < computerCount; j++) computers[j].traversed = 0;
		getMaxCircularConnections(&computers[i], computers[i].connections, computers[i].connectionCount);
	}
	//
	printf("Computer maxsize total: %llu\n", maxStackCount);
	//
	bool sorted = false;
	while (!sorted) {
		sorted = true;
		for (int i = 1; i < maxStackCount; i++) {
			if (maxStack[i - 1]->name[0] < maxStack[i]->name[0] ||
				(maxStack[i - 1]->name[0] == maxStack[i]->name[0] && maxStack[i - 1]->name[1] <= maxStack[i]->name[1])) continue;
			sorted = false;

			Computer* temp = maxStack[i - 1];
			maxStack[i - 1] = maxStack[i];
			maxStack[i] = temp;
		}
	}
	//
	for (int i = 0; i < maxStackCount; i++) {
		if (i > 0) printf(",");
		printf("%c%c", maxStack[i]->name[0], maxStack[i]->name[1]);
	}
}