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

			bool matching = false;
			const uint64_t len = strlen(buffer);
			const uint64_t mid = len / 2;
			for (uint64_t j = 2; j <= len && !matching; j++) {
				if (len % j != 0)
					continue;
				matching = true;

				const uint64_t factor = len / j;
				char* buf = buffer;
				for (uint64_t k = 1; k < j && matching; k++) {
					if (memcmp(buf, buf + factor, factor) != 0)
						matching = false;
					buf += factor;
				}
			}
			if (!matching)
				continue;

			printf("Found %i\n", i);
			idSum += i;
		}
	}

	printf("ID sum: %llu\n", idSum);
}