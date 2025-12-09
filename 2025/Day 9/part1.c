#include <common/common.h>

#define BOX_MAX 1000
typedef struct {
	int64_t x, y;
} Box;
Box boxes[BOX_MAX];
size_t boxCount = 0;

int64_t rectsize(Box* a, Box* b) {
	int64_t cx = b->x - a->x,
			cy = b->y - a->y;
	if (cx < 0) cx = -cx;
	if (cy < 0) cy = -cy;
	return (cx + 1) * (cy + 1);
}

int main() {
	FileReader fileReader = FileReader_getInput();

	while (!fileReader.endReached) {
		FileReader_readUntilDelim(&fileReader, "\n");
		sscanf(fileReader.readBuffer, "%lli,%lli", &boxes[boxCount].x, &boxes[boxCount].y);
		boxCount++;
	}

	int64_t bestSize = 0;
	for (int i = 0; i < boxCount; i++) {
		for (int j = i + 1; j < boxCount; j++) {
			int64_t size = rectsize(&boxes[i], &boxes[j]);
			if (bestSize >= size)
				continue;
			bestSize = size;
		}
	}
	printf("Best: %lli\n", bestSize);
}