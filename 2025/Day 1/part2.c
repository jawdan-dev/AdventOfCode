#include <common/common.h>

int main() {
	FileReader fileReader = FileReader_getInput();

	int position = 50, passwordCount = 0;
	while (!fileReader.endReached) {
		FileReader_readUntilDelim(&fileReader, "\n");

		char direction;
		int amount;
		sscanf(fileReader.readBuffer, "%c%i\n", &direction, &amount);

		for (int i = 0; i < amount; i++) {
			switch (direction) {
				case 'R': position++; break;
				case 'L': position--; break;
			}

			while (position < 0) {
				position += 100;
			}
			while (position >= 100) {
				position -= 100;
			}

			if (position == 0)
				passwordCount++;
		}
	}

	printf("Password count: %i\n", passwordCount);
}