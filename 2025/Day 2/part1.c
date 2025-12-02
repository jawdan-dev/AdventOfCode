#include <common/common.h>

int main() {
	FileReader fileReader = FileReader_getInput();

	uint64_t idSum = 0;
	while (!fileReader.endReached) {
		FileReader_readUntilDelim(&fileReader, "\n,");

		uint64_t firstID, secondID;
		if (sscanf(fileReader.readBuffer, "%llu-%llu\n", &firstID, &secondID) != 2)
			continue;

		char buffer[32];
		for (uint64_t i = firstID; i <= secondID; i++) {
			sprintf(buffer, "%llu", i);

			const uint64_t len = strlen(buffer);
			if (len % 2 == 1)
				continue;

			const uint64_t mid = len / 2;
			if (memcmp(buffer, buffer + mid, mid) != 0)
				continue;

			printf("Found %i\n", i);
			idSum += i;
		}
	}

	printf("ID sum: %llu\n", idSum);
}