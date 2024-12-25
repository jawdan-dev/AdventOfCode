#include <common/common.h>
#include <common/point.h>

int main() {
	FileReader fileReader = FileReader_getInput();
	SafeType niceCount = 0;
	while (!fileReader.endReached) {
		FileReader_readUntilDelim(&fileReader, "\n");
		//
		int doubleStringCount = 0;
		for (int i = 1; i < fileReader.readBufferLength; i++)
			for (int j = i + 2; j < fileReader.readBufferLength; j++)
				if (fileReader.readBuffer[i - 1] == fileReader.readBuffer[j - 1] &&
					fileReader.readBuffer[i] == fileReader.readBuffer[j])
					doubleStringCount++;
		//
		int sandwichCount = 0;
		for (int i = 2; i < fileReader.readBufferLength; i++)
			if (fileReader.readBuffer[i - 2] == fileReader.readBuffer[i])
				sandwichCount++;
		//
		if (doubleStringCount >= 1 && sandwichCount >= 1) 
			niceCount++;
	}
	printf("Nice strings: %lli\n", niceCount);
}