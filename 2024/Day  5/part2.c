#include <common/common.h>

int main() {
	printf("\n\n\n");
	FileReader fileReader = FileReader_getInput();

	// Read in mapping.
	typedef struct {
		int value, before;
	} Mapping;
#	define MAPPING_MAX 2048
	Mapping mapping[MAPPING_MAX];
	int mappingCount = 0;

	while (!fileReader.endReached) {
		FileReader_readUntilDelim(&fileReader, "\n");
		if (fileReader.readBufferLength == 0) break;

		// Read mapping.
		sscanf(fileReader.readBuffer, "%i|%i", &mapping[mappingCount].value, &mapping[mappingCount].before);
		mappingCount++;
	}

	// Read and process lines.
#	define LINE_MAX 1024
	int line[LINE_MAX];
	int lineCount = 0, totalMiddlePages = 0;
	while (!fileReader.endReached) {
		FileReader_readUntilDelim(&fileReader, "\n");

		// Get line values.
		lineCount = 0;
		while (FileReader_readNextInt(&fileReader, &line[lineCount])) lineCount++;

		// Process line.
		bool innatelyOrdered = true;
		for (int n = lineCount - 1; n >= 0; n--) {
			const int movePage = line[n];

			bool remmapped = false;
			for (int i = 0; i < n && !remmapped; i++) {
				for (int j = 0; j < mappingCount && !remmapped; j++) {
					if (mapping[j].value != movePage || mapping[j].before != line[i]) continue;

					// Remmap.
					const int insertIndex = i;
					for (int k = n; k > insertIndex; k--)
						line[k] = line[k - 1];
					line[insertIndex] = movePage;

					// Reset.
					innatelyOrdered = false;
					remmapped = true;
					n = lineCount;
				}
			}
		}
		// Only print if not innately ordered correctly.
		if (!innatelyOrdered) totalMiddlePages += line[lineCount / 2];
	}
	printf("Middle page sum: %i\n", totalMiddlePages);
}