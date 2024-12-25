#include <common/common.h>

int main() {
	FileReader fileReader = FileReader_getInput();
	FileReader_readUntilDelim(&fileReader, "\n");
	//
	SafeType floor = 0;
	for (int i = 0; i < fileReader.readBufferLength; i++) 
		floor += fileReader.readBuffer[i] == '(' ? 1 : -1;
	//
	printf("Floor: %lli\n", floor);
}