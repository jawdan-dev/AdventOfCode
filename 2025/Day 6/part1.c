#include <common/common.h>

#define TABLE_ROW_MAX 16
#define TABLE_COLUMN_MAX 4000
char table[TABLE_ROW_MAX][TABLE_COLUMN_MAX];
size_t rowCount = 0, columnCount = 0;

const uint64_t getValue(size_t col) {
	uint64_t value = 0;
	for (int i = 0; i < rowCount; i++) {
		if (table[i][col] == ' ')
			continue;
		value *= 10;
		value += table[i][col] - '0';
	}
	return value;
}

int main() {
	FileReader fileReader = FileReader_getInput();

	while (!fileReader.endReached) {
		FileReader_readUntilDelim(&fileReader, "\n");
		strcpy(&table[rowCount][0], fileReader.readBuffer);
		columnCount = max(columnCount, strlen(&table[rowCount][0]));
		rowCount++;
	}
	rowCount--;

	uint64_t total = 0;
	for (int col = 0; col < columnCount;) {
		const char symbol = table[rowCount][col];
		uint64_t value = getValue(col++);

		uint64_t temp;
		while (col < columnCount && (temp = getValue(col++)) != 0) {
			switch (symbol) {
				case '+':
					value += temp;
					break;
				case '*':
					value *= temp;
					break;
			}
		}
		total += value;
	}
	printf("Total Sum: %llu\n", total);
}