// Load FileBuffer.
#define FILEBUFFER_READBUFFERSIZE 100
enum ReadState {
	ReadState_Line,
};
#include "../Common/FileBuffer.h"

const size_t hash(const char* str, const size_t len);

#define LABEL_MAX 10

struct Lens {
	char label[LABEL_MAX];
	size_t focalLength;
};

typedef struct Box Box;
struct Box {
	struct Lens lenses[20];
	size_t lensCount;
};

// Entry point.
int main() {
	// Create file buffer.
	struct FileBuffer fileBuffer;
	FileBuffer_OpenFile(&fileBuffer, "input.txt");

	// File delims.
	struct FileDelim delims[] = {
		{ ',', ReadState_Line }
	};
	FileBuffer_SetDelims(&fileBuffer, delims, sizeof(delims) / sizeof(*delims), ReadState_Line);

	// Interpretation variables.
	Box boxes[256];
	for (size_t i = 0; i < sizeof(boxes) / sizeof(*boxes); i++) {
		boxes[i].lensCount = 0;
	}

	// Interpret file.
	while (FileBuffer_ReadUntilNextDelim(&fileBuffer)) {
		// Processing.
		switch (fileBuffer.readState) {
			case ReadState_Line: {
				// Get lable.
				char label[LABEL_MAX];
				size_t labelSize = 0;
				while (fileBuffer.readBuffer[labelSize] != '-' && fileBuffer.readBuffer[labelSize] != '=') {
					label[labelSize] = fileBuffer.readBuffer[labelSize];
					labelSize++;
				}
				label[labelSize] = '\0';

				// Get box.
				const size_t boxIndex = hash(label, labelSize);
				Box* box = boxes + boxIndex;

				switch (fileBuffer.readBuffer[labelSize]) {
					case '-': {
						for (size_t i = 0; i < box->lensCount; i++) {
							if (strcmp(box->lenses[i].label, label) != 0) continue;

							// Remove lens.
							for (size_t j = i + 1; j < box->lensCount; j++) {
								memcpy(box->lenses + j - 1, box->lenses + j, sizeof(*box->lenses));
							}

							// Update details.
							box->lensCount--;
							break;
						}
					} break;
					case '=': {
						size_t focalLength = 0;
						sscanf(fileBuffer.readBuffer + labelSize + 1, "%zu", &focalLength);

						bool found = false;
						for (size_t i = 0; i < box->lensCount && !found; i++) {
							if (strcmp(box->lenses[i].label, label) != 0) continue;
							found = true;
							// Swap lens.
							box->lenses[i].focalLength = focalLength;
						}

						if (found) break;

						if (box->lensCount > sizeof(box->lenses) / sizeof(*box->lenses)) {
							printf("Max lens count exceeded.\n");
							abort();
						}

						// Add lens.
						box->lenses[box->lensCount].focalLength = focalLength;
						strcpy(box->lenses[box->lensCount].label, label);
						box->lensCount++;
					} break;
				}
			} break;
		}
	}

	// Calculate total.
	size_t totalFocusingPower = 0;
	for (size_t i = 0; i < sizeof(boxes) / sizeof(*boxes); i++) {
		if (boxes[i].lensCount <= 0) continue;

		printf("box %i:\n", i);
		for (size_t j = 0; j < boxes[i].lensCount; j++) {
			// Calculate lens power.
			const size_t focalLength = boxes[i].lenses[j].focalLength;
			const size_t lensPower = (i + 1) * (j + 1) * focalLength;
			// Add to total.
			totalFocusingPower += lensPower;

			printf("\t[%c%c %zu] > %zu\n", boxes[i].lenses[j].label[0], boxes[i].lenses[j].label[1], focalLength, lensPower);
		}
	}

	// Output answer.
	printf("--------------\n");
	printf("Output: %zu\n", totalFocusingPower);
	return 0;
}

const size_t hash(const char* str, const size_t len) {
	size_t hash = 0;
	for (size_t i = 0; i < len; i++) {
		hash += (size_t)str[i];
		hash *= 17;
		hash %= 256;
	}
	return hash;
}