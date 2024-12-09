#include <common/common.h>

typedef signed long long ValueType;
//
char* disk;
bool* diskHandled;
ValueType diskLength;
//
ValueType leftHead, rightHead;
const bool findRightOfLength(ValueType minLength, ValueType* out, ValueType* count) {
	while (disk[rightHead] <= 0) rightHead -= 2;
	if (rightHead < 0) return false;

	for (ValueType i = rightHead; i > leftHead; i -= 2) {
		if (!diskHandled[i] && 0 < disk[i] && disk[i] <= minLength) {
			*out = i / 2;
			*count = disk[i];
			diskHandled[i] = true;
			return true;
		}
	}
	return false;
}

int main() {
	FileReader fileReader = FileReader_getInput();
	FileReader_readUntilDelim(&fileReader, "");
	disk = fileReader.readBuffer;
	diskLength = fileReader.readBufferLength;
	diskHandled = malloc(sizeof(bool) * diskLength);
	for (int i = 0; i < diskLength; i++) {
		disk[i] -= '0';
		diskHandled[i] = false;
	}
	//
	ValueType total = 0;
	//
	ValueType offset = 0;
	leftHead = 0, rightHead = diskLength - (diskLength % 2);

	while (leftHead < diskLength) {
		if (leftHead % 2 == 0 && !diskHandled[leftHead]) {
			const ValueType leftId = leftHead / 2;
			while (disk[leftHead] > 0) {
				total += leftId * offset++;
				disk[leftHead]--;
			}
			leftHead++;
		} else {
			ValueType right, count;
			const bool found = findRightOfLength(disk[leftHead], &right, &count);
			while (disk[leftHead] > 0 && count > 0) {
				total += right * offset++;
				disk[leftHead]--;
				count--;
			}
			if (!found) offset += disk[leftHead];
			if (disk[leftHead] <= 0 || !found) leftHead++;
		}
	}
	printf("Disk checksum total: %llu\n", total);
}