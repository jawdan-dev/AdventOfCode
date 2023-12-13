// Load FileBuffer.
#define FILEBUFFER_READBUFFERSIZE 100
enum ReadState {
	ReadState_Line,
};
#include "../Common/FileBuffer.h"

// Entry point.
int main() {
	// Create file buffer.
	struct FileBuffer fileBuffer;
	FileBuffer_OpenFile(&fileBuffer, "input.txt");

	// File delims.
	struct FileDelim delims[] = {
		{ '\n', ReadState_Line }
	};
	FileBuffer_SetDelims(&fileBuffer, delims, sizeof(delims) / sizeof(*delims), ReadState_Line);

	// Interpretation variables.
	size_t totalScore = 0;

	size_t bufferWidth = 0, bufferHeight = 0;
	char rockBuffer[FILEBUFFER_READBUFFERSIZE][FILEBUFFER_READBUFFERSIZE];

	// Interpret file.
	while (FileBuffer_ReadUntilNextDelim(&fileBuffer)) {
		// Processing.
		switch (fileBuffer.readState) {
			case ReadState_Line: {
				const size_t bufferSize = strlen(fileBuffer.readBuffer);
				if (bufferSize != 0) {
					// Copy data.
					for (size_t i = 0; i < bufferSize; i++)
						rockBuffer[i][bufferHeight] = fileBuffer.readBuffer[i];

					printf("%s\n", fileBuffer.readBuffer);

					// Update details.
					bufferHeight++;
					if (bufferWidth < bufferSize)
						bufferWidth = bufferSize;

					break;
				}

				// Interpretation values.
				size_t mirrorScore = 0;
				bool mirrored;
#				define SMUDGE_MAX 1
				size_t smudgeCount;

				// Check for vertical mirror.
				mirrored = false;
				for (size_t i = 1; i < bufferWidth && !mirrored; i++) {
					mirrored = true;
					smudgeCount = 0;
					for (size_t k = 0; mirrored && k < bufferHeight; k++)
						for (size_t j = 0; mirrored && j < i && (i + j) < bufferWidth && (i - (j + 1)) < bufferWidth; j++)
							// Compare characters.
							if (rockBuffer[i - (j + 1)][k] != rockBuffer[i + j][k])
								// Check for smudges.
								if (smudgeCount < SMUDGE_MAX) smudgeCount++;
								else mirrored = false;

					// Only accept smudged mirrors.
					if (smudgeCount == 0)
						mirrored = false;

					// Check if mirrored.
					if (mirrored) mirrorScore = i;
				}

				// Check for horizontal mirror.
				for (size_t i = 1; i < bufferHeight && !mirrored; i++) {
					mirrored = true;
					smudgeCount = 0;
					for (size_t k = 0; mirrored && k < bufferWidth; k++)
						for (size_t j = 0; mirrored && j < i && (i + j) < bufferHeight && (i - (j + 1)) < bufferHeight; j++)
							// Compare characters.
							if (rockBuffer[k][i - (j + 1)] != rockBuffer[k][i + j])
								// Check for smudges.
								if (smudgeCount < SMUDGE_MAX) smudgeCount++;
								else mirrored = false;

					// Only accept smudged mirrors.
					if (smudgeCount == 0)
						mirrored = false;

					// Check if mirrored.
					if (mirrored) mirrorScore = i * 100;
				}

				// Check if mirror found.
				if (mirrored)
					// Update score.
					totalScore += mirrorScore;

				// Reset details.
				bufferWidth = 0;
				bufferHeight = 0;
			} break;
		}
	}

	// Output answer.
	printf("Output: %zu\n", totalScore);
	return 0;
}

