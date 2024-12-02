#include <common/common.h>


int main() {
	printf("\n\n\n");
	FileReader fileReader = FileReader_getInput();

#	define REPORT_MAX 1000
	int report[REPORT_MAX];
	size_t reportLength = 0;

	int safeCount = 0;
	while (!fileReader.endReached) {
		FileReader_readUntilDelim(&fileReader, "\n");

		reportLength = 0;
		int value = 0;
		for (int i = 0; i < fileReader.readBufferLength; i++) {
			while (i < fileReader.readBufferLength && fileReader.readBuffer[i] != ' ') {
				value *= 10;
				value += fileReader.readBuffer[i] - '0';
				i++;
			}
			report[reportLength++] = value;
			value = 0;
		}

		bool safe = true;
		int direction = 0;
		for (int i = 1; i < reportLength && safe; i++) {
			if (report[i - 1] < report[i]) {
				if (direction < 0)
					safe = false;
				direction = 1;
			} else if (report[i - 1] > report[i]) {
				if (direction > 0)
					safe = false;
				direction = -1;
			}

			const int change = abs(report[i - 1] - report[i]);
			if (change == 0 || change > 3)
				safe = false;
		}
		if (safe) safeCount++;
	}

	printf("Safe count: %i\n", safeCount);
}