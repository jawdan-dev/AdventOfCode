#include <common/common.h>

int main() {
	FileReader fileReader = FileReader_getInput();
	//
	SafeType characterDifference = 0;
	while (!fileReader.endReached) {
		FileReader_readUntilDelim(&fileReader, "\n");
		//
		SafeType totalVisibleCharacters = 2;
		for (int i = 0; i < fileReader.readBufferLength; i++) {
			switch (fileReader.readBuffer[i]) {
				default: totalVisibleCharacters++; break;
				case '\"': 
				case '\\': totalVisibleCharacters += 2; break;
			}
		}
		characterDifference += (totalVisibleCharacters - fileReader.readBufferLength);
	}
	//
	printf("Character difference: %lli\n", characterDifference);
}