#include <common/common.h>

int main() {
	FileReader fileReader = FileReader_getInput();

#define TABLE_ROW_MAX 16
#define TABLE_COLUMN_MAX 1024
	char table[TABLE_ROW_MAX][TABLE_COLUMN_MAX];
	size_t rowCount = 0, columnCount = 0;

	while (!fileReader.endReached) {
		FileReader_readUntilDelim(&fileReader, "\n");

		columnCount = 0;

		int index = 0;
		int value = 0;
		bool hasValue = false;
		while (index < fileReader.readBufferLength) {
			hasValue = false;
			while (index < fileReader.readBufferLength && fileReader.readBuffer[index] == ' ')
				index++;

			while (index < fileReader.readBufferLength && fileReader.readBuffer[index] != ' ') {
				switch (fileReader.readBuffer[index]) {
					case '+':
					case '*':
						symbols[columnCount++] = fileReader.readBuffer[index];
						break;

					default:
						value *= 10;
						value += fileReader.readBuffer[index] - '0';
						hasValue = true;
						break;
				}
				index++;
			}

			if (hasValue) {
				table[rowCount][columnCount++] = value;
				hasValue = false;
				value = 0;
			}
		}
		rowCount++;
	}
	rowCount--;

	uint64_t total = 0;
	for (int col = 0; col < columnCount; col++) {
		uint64_t value = 0;
		value = table[0][col];
		printf("%llu ", table[0][col]);
		for (int row = 1; row < rowCount; row++) {
			switch (symbols[col]) {
				case '+':
					value += table[row][col];
					break;
				case '*':
					value *= table[row][col];
					break;
			}
			printf("%c %llu ", symbols[col], table[row][col]);
		}
		printf("= %llu\n", value);
		total += value;
	}
	printf("Total Sum: %llu\n", total);
}