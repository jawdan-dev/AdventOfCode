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
	if (cx < 0)
		cx = -cx;
	if (cy < 0)
		cy = -cy;
	return (cx + 1) * (cy + 1);
}

bool isSafe(size_t a, size_t b) {
	const int64_t minx = min(boxes[a].x, boxes[b].x),
				  miny = min(boxes[a].y, boxes[b].y),
				  maxx = max(boxes[a].x, boxes[b].x),
				  maxy = max(boxes[a].y, boxes[b].y);

	for (size_t i = 0; i < boxCount; i++) {
		size_t ni = (i + 1) % boxCount;

		const int64_t minix = min(boxes[i].x, boxes[ni].x),
					  miniy = min(boxes[i].y, boxes[ni].y),
					  maxix = max(boxes[i].x, boxes[ni].x),
					  maxiy = max(boxes[i].y, boxes[ni].y);

		// Simple AABB test? yeah okay,, this was simpler than i thought....
		if (minx < maxix &&
			maxx > minix &&
			miny < maxiy &&
			maxy > miniy)
			return false;
	}
	return true;
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
			if (!isSafe(i, j))
				continue;
			int64_t size = rectsize(&boxes[i], &boxes[j]);
			if (bestSize >= size)
				continue;
			bestSize = size;
		}
	}
	printf("Best: %lli\n", bestSize);
}