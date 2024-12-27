#include <common/common.h>

#define BUFFER_MAX 10000000
char buffer[2][BUFFER_MAX];

int depth = 0;
void expandBuffer() {
	char* readBuffer = (char*)buffer[depth % 2];
	depth++;
	char* writeBuffer = (char*)buffer[depth % 2];

	int lastWrite = 0;
	writeBuffer[lastWrite] = '\0';

	const int readLen = strlen(readBuffer);
	for (int i = 0; i < readLen; i++) {
		int count = 0;
		while (i + count < readLen && readBuffer[i] == readBuffer[i + count]) count++;
		lastWrite += strlen(&writeBuffer[lastWrite]);
		sprintf(&writeBuffer[lastWrite], "%i%c", count, readBuffer[i]);
		i += count - 1;
	}

	printf("%i: %i\n", depth, strlen(writeBuffer));
}

int main() {
	FileReader fileReader = FileReader_getInput();
	FileReader_readUntilDelim(&fileReader, "\n");
	//
	strcpy(buffer[0], fileReader.readBuffer);
	//
	for (int i = 0; i < 50; i++) expandBuffer();
	//
	printf("Result length: %i\n", strlen(buffer[depth % 2]));
}