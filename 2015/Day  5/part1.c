#include <common/common.h>
#include <common/point.h>

const char* vowels = "aeiou";
const char* badStrings[] = { "ab", "cd", "pq", "xy" };

int main() {
	FileReader fileReader = FileReader_getInput();
	SafeType niceCount = 0;
	while (!fileReader.endReached) {
		FileReader_readUntilDelim(&fileReader, "\n");
		// 
		int vowelCount = 0;
		for (int j = 0; j < strlen(vowels); j++)
			for (int i = 0; i < fileReader.readBufferLength; i++)	
				if (fileReader.readBuffer[i] == vowels[j])
					vowelCount++;
		//
		int doubleCharacterCount = 0;
		for (int i = 1; i < fileReader.readBufferLength; i++)
			if (fileReader.readBuffer[i - 1] == fileReader.readBuffer[i])
				doubleCharacterCount++;
		//
		int badStringCount = 0;
		for (int j = 0; j < ARRAY_SIZE(badStrings); j++) {
			for (int i = 0; i <=	 fileReader.readBufferLength - strlen(badStrings[j]); i++) {
				bool matched = true;
				for (int k = 0; k < strlen(badStrings[j]) && matched; k++)
					if (fileReader.readBuffer[i + k] != badStrings[j][k])
						matched = false;
				if (matched) badStringCount++;
			}
		}
		//
		if (vowelCount >= 3 && doubleCharacterCount >= 1 && badStringCount <= 0) 
			niceCount++;
	}
	printf("Nice strings: %lli\n", niceCount);
}