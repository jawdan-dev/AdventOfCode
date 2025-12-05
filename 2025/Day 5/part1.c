#include <common/common.h>

int main() {
	FileReader fileReader = FileReader_getInput();

	struct FreshInfo {
		uint64_t min, max;
	} fresh[256];
	size_t freshCount = 0;

	while (!fileReader.endReached) {
		FileReader_readUntilDelim(&fileReader, "\n");
		if (strlen(fileReader.readBuffer) == 0)
			break;

		sscanf(fileReader.readBuffer, "%llu-%llu", &fresh[freshCount].min, &fresh[freshCount].max);
		freshCount++;
	}

	size_t freshTotal = 0;
	while (!fileReader.endReached) {
		FileReader_readUntilDelim(&fileReader, "\n");
		uint64_t value;
		sscanf(fileReader.readBuffer, "%llu", &value);
		for (size_t i = 0; i < freshCount; i++) {
			if (fresh[i].min <= value && value <= fresh[i].max) {
				freshTotal++;
				break;
			}
		}
	}
	printf("Total Fresh: %zu\n", freshTotal);
}