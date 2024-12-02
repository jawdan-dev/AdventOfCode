#include <common/common.h>


const bool isReportSafe(const int* const report, const size_t reportLength) {
	int direction = 0;
	for (int i = 1; i < reportLength; i++) {
		const int change = abs(report[i - 1] - report[i]);
		if (change == 0 || change > 3)
			return false;

		if (report[i - 1] < report[i]) {
			if (direction < 0)
				return false;
			direction = 1;
		} else if (report[i - 1] > report[i]) {
			if (direction > 0)
				return false;
			direction = -1;
		}
	}

	return true;
}

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

		bool safe = isReportSafe(report, reportLength);
		for (int i = 0; i < reportLength && !safe; i++) {
			const int temp = report[i];
			for (int j = i + 1; j < reportLength; j++)
				report[j - 1] = report[j];
			reportLength--;

			if (isReportSafe(report, reportLength))
				safe = true;

			reportLength++;
			for (int j = reportLength - 1; j > i; j--)
				report[j] = report[j - 1];
			report[i] = temp;
		}

		if (safe)
			safeCount++;
	}

	printf("Safe count: %i\n", safeCount);
}