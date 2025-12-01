#include <common/common.h>

int main() {
	FileReader fileReader = FileReader_getInput();

	int position = 50, passwordCount = 0;
	while (!fileReader.endReached) {
		FileReader_readUntilDelim(&fileReader, "\n");

		char direction;
		int amount;
		sscanf(fileReader.readBuffer, "%c%i\n", &direction, &amount);

		switch (direction) {
			case 'R': position += amount; break;
			case 'L': position += 100 - (amount % 100); break;
		}

		position = position % 100;
		if (position % 100 == 0)
			passwordCount++;
	}

	printf("Password count: %i\n", passwordCount);
}