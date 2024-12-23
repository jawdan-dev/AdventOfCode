#include <common/common.h>

typedef long long ValueType;

#define CONNECTIONS_MAX 200
#define COMPUTERS_MAX 4000

typedef struct Computer {
	char name[2];
	struct Computer* connections[CONNECTIONS_MAX];
	int connectionCount;
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

const int getMaxCircularConnections(Computer* this, Computer* root, const int remainingDepth, const bool valid) {
	if (remainingDepth <= 0) return this == root && valid;

	int totalConnections = 0;
	for (int i = 0; i < this->connectionCount; i++)
		totalConnections += getMaxCircularConnections(this->connections[i], root, remainingDepth - 1, valid || this->name[0] == 't');
	return totalConnections;
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
	int total = 0;
	for (int i = 0; i < computerCount; i++)
		total += getMaxCircularConnections(&computers[i], &computers[i], 3, false);
	//
	printf("Computer total: %llu\n", total / 6);
}