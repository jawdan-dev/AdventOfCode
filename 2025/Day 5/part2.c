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

	for (size_t i = 0; i < freshCount; i++) {
		for (size_t j = i + 1; j < freshCount; j++) {
			if (fresh[i].min <= fresh[j].max && fresh[i].max >= fresh[j].min) {
				fresh[i].min = min(fresh[i].min, fresh[j].min);
				fresh[i].max = max(fresh[i].max, fresh[j].max);

				for (size_t k = j + 1; k < freshCount; k++) {
					fresh[k - 1] = fresh[k];
				}
				freshCount--;

				j = i;
				continue;
			}
		}
	}

	uint64_t freshTotal = 0;
	for (size_t i = 0; i < freshCount; i++) {
		freshTotal += (fresh[i].max - fresh[i].min) + 1;
	}
	printf("Total Fresh: %llu\n", freshTotal);
}