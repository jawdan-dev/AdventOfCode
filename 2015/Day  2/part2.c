#include <common/common.h>

int main() {
	FileReader fileReader = FileReader_getInput();
	//
	SafeType totalPaper = 0;
	while (!fileReader.endReached) {
		FileReader_readUntilDelim(&fileReader, "\n");
		//
		int l, w, h;
		sscanf(fileReader.readBuffer, "%ix%ix%i", &l, &w, &h);
		//
		totalPaper += min(min(l + w, w + h), h + l) * 2;
		totalPaper += l * w * h;
	}
	//
	printf("Total paper: %lli\n", totalPaper);
}