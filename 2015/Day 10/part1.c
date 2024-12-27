#include <common/common.h>

#define BUFFER_MAX 1000000
char buffer[BUFFER_MAX];
char tempBuffer[BUFFER_MAX];

void expandBuffer() {
	tempBuffer[0] = '\0';

	const int bufferLen = strlen(buffer);
	for (int i = 0; i < bufferLen; i++) {
		int count = 0;
		while (i + count < bufferLen && buffer[i] == buffer[i + count]) count++;
		sprintf(&tempBuffer[strlen(tempBuffer)], "%i%c", count, buffer[i]);
		i += count - 1;
	}

	printf("%i\n", strlen(tempBuffer));
	strcpy(buffer, tempBuffer);
}

int main() {
	FileReader fileReader = FileReader_getInput();
	FileReader_readUntilDelim(&fileReader, "\n");
	//
	strcpy(buffer, fileReader.readBuffer);
	//
	for (int i = 0; i < 40; i++) expandBuffer();
	//
	printf("Result length: %i\n", strlen(buffer));
}