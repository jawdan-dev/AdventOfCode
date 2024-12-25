#include <common/common.h>

int main() {
	FileReader fileReader = FileReader_getInput();
	FileReader_readUntilDelim(&fileReader, "\n");
	//
	SafeType floor = 0;
	for (int i = 0; i < fileReader.readBufferLength; i++) {
		floor += fileReader.readBuffer[i] == '(' ? 1 : -1;
		if (floor > -1) continue;
		printf("First basement index: %lli\n", i - 1);
		return;
	}
}