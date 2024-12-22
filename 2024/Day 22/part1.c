#include <common/common.h>

typedef long long ValueType;

const ValueType getSecretValue(const ValueType input, const ValueType iterations) {
	if (iterations <= 0) return input;

	ValueType result = input;
	result = (result ^ (result * 64)) % 16777216;
	result = (result ^ (result / 32)) % 16777216;
	result = (result ^ (result * 2048)) % 16777216;

	return getSecretValue(result, iterations - 1);
}

int main() {
	FileReader fileReader = FileReader_getInput();
	//
	ValueType total = 0;
	while (!fileReader.endReached) {
		FileReader_readUntilDelim(&fileReader, "\n");
		//
		ValueType value = 0;
		if (!FileReader_readNextLLU(&fileReader, &value)) continue;
		//
		total += getSecretValue(value, 2000);
	}
	//
	printf("Secret total: %llu\n", total);
}