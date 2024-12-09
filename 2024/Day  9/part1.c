#include <common/common.h>

typedef signed long long ValueType;
//
char* disk;
ValueType diskLength;
//
ValueType leftHead, rightHead;
const bool popRight(ValueType* out) {
	while (disk[rightHead] <= 0) rightHead -= 2;
	if (rightHead < 0) return false;
	//
	disk[rightHead]--;
	//
	*out = rightHead / 2;
	return true;
}

int main() {
	FileReader fileReader = FileReader_getInput();
	FileReader_readUntilDelim(&fileReader, "");
	disk = fileReader.readBuffer;
	diskLength = fileReader.readBufferLength;
	for (int i = 0; i < diskLength; i++) disk[i] -= '0';
	//
	ValueType total = 0;
	//
	ValueType offset = 0;
	leftHead = 0, rightHead = diskLength - (diskLength % 2);

	while (leftHead < diskLength) {
		if (leftHead % 2 == 0) {
			const ValueType leftId = leftHead / 2;
			while (disk[leftHead] > 0) {
				total += leftId * offset++;
				disk[leftHead]--;
			}
		} else {
			while (disk[leftHead] > 0) {
				ValueType right;
				if (!popRight(&right)) break;
				total += right * offset++;
				disk[leftHead]--;
			}
		}
		leftHead++;
	}
	printf("Disk checksum total: %llu\n", total);
}