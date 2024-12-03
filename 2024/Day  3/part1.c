#include <common/common.h>

int main() {
	printf("\n\n\n");
	FileReader fileReader = FileReader_getInput();

	size_t multiplicationSum = 0;
	while (!fileReader.endReached) {
		FileReader_readUntilDelim(&fileReader, "\n");

		int a, b;
		for (int i = 0; i < fileReader.readBufferLength; i++) {
			char* str = fileReader.readBuffer + i;

			if (strncmp(str, "mul(", 4) != 0) continue;
			str += 4;

			a = 0;
			if (!('0' <= *str && *str <= '9')) continue;
			while ('0' <= *str && *str <= '9') {
				a *= 10;
				a += *str - '0';
				str++;
			}

			if (*str != ',') continue;
			str++;

			b = 0;
			if (!('0' <= *str && *str <= '9')) continue;
			while ('0' <= *str && *str <= '9') {
				b *= 10;
				b += *str - '0';
				str++;
			}

			if (*str != ')') continue;

			multiplicationSum += a * b;
			i = str - fileReader.readBuffer;

			printf("%i, %i\n", a, b);
		}
	}

	printf("Multiplication sum: %llu\n", multiplicationSum);
}