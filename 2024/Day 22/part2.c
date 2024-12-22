#include <common/common.h>

typedef long long ValueType;

#define SEQUENCE_SIZE 21
#define SEQUENCE_OFFSET -9
bool sequenceChanged[SEQUENCE_SIZE][SEQUENCE_SIZE][SEQUENCE_SIZE][SEQUENCE_SIZE];
ValueType sequenceTotals[SEQUENCE_SIZE][SEQUENCE_SIZE][SEQUENCE_SIZE][SEQUENCE_SIZE];

void getSecretValue(const ValueType input, const ValueType iterations, ValueType changeSequence[4]) {
	bool sequenceValid = true;
	for (int i = 0; i < 4 && sequenceValid; i++)
		if (changeSequence[i] == -10)
			sequenceValid = false;
	const int a = changeSequence[0] - SEQUENCE_OFFSET,
		b = changeSequence[1] - SEQUENCE_OFFSET,
		c = changeSequence[2] - SEQUENCE_OFFSET,
		d = changeSequence[3] - SEQUENCE_OFFSET;
	if (sequenceValid && !sequenceChanged[a][b][c][d]) {
		sequenceTotals[a][b][c][d] += input % 10;
		sequenceChanged[a][b][c][d] = true;
	}
	if (iterations <= 0) return;

	ValueType result = input;
	result = (result ^ (result * 64)) % 16777216;
	result = (result ^ (result / 32)) % 16777216;
	result = (result ^ (result * 2048)) % 16777216;

	for (int i = 1; i < 4; i++) changeSequence[i - 1] = changeSequence[i];
	changeSequence[4 - 1] = (result % 10) - (input % 10);

	getSecretValue(result, iterations - 1, changeSequence);
}

int main() {
	FileReader fileReader = FileReader_getInput();
	//
#	define SECRET_MAX 2024
	ValueType secrets[SECRET_MAX];
	int secretCount = 0;
	//
	while (!fileReader.endReached) {
		FileReader_readUntilDelim(&fileReader, "\n");
		//
		if (FileReader_readNextLLU(&fileReader, &secrets[secretCount]))
			secretCount++;
	}
	//
	for (int a = 0; a < SEQUENCE_SIZE; a++)
		for (int b = 0; b < SEQUENCE_SIZE; b++)
			for (int c = 0; c < SEQUENCE_SIZE; c++)
				for (int d = 0; d < SEQUENCE_SIZE; d++)
					sequenceTotals[a][b][c][d] = 0;
	//
	for (int i = 0; i < secretCount; i++) {
		for (int a = 0; a < SEQUENCE_SIZE; a++)
			for (int b = 0; b < SEQUENCE_SIZE; b++)
				for (int c = 0; c < SEQUENCE_SIZE; c++)
					for (int d = 0; d < SEQUENCE_SIZE; d++)
						sequenceChanged[a][b][c][d] = false;
		ValueType baseSequence[4] = { -10, -10, -10, -10 };
		getSecretValue(secrets[i], 2000, baseSequence);
	}
	//
	ValueType maxTotal = 0;
	for (int a = 0; a < SEQUENCE_SIZE; a++)
		for (int b = 0; b < SEQUENCE_SIZE; b++)
			for (int c = 0; c < SEQUENCE_SIZE; c++)
				for (int d = 0; d < SEQUENCE_SIZE; d++)
					if (maxTotal < sequenceTotals[a][b][c][d])
						maxTotal = sequenceTotals[a][b][c][d];
	//
	printf("Max sequencial secret total: %llu\n", maxTotal);
}