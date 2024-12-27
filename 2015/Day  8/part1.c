#include <common/common.h>

int main() {
	FileReader fileReader = FileReader_getInput();
	//
	SafeType characterDifference = 0;
	while (!fileReader.endReached) {
		FileReader_readUntilDelim(&fileReader, "\n");
		//
		SafeType totalVisibleCharacters = -2;
		for (int i = 0; i < fileReader.readBufferLength; i++) {
			totalVisibleCharacters++;
			if (fileReader.readBuffer[i] != '\\') continue;

			switch (fileReader.readBuffer[++i]) {
				case 'x': i += 2; break;
			}
		}
		characterDifference += (fileReader.readBufferLength - totalVisibleCharacters);
	}
	//
	printf("Character difference: %lli\n", characterDifference);
}