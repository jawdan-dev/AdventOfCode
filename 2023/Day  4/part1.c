#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#define nullptr NULL

enum ReadState {
	GameID,
	WinningNumbers, OwnedNumbers
};

int main() {
	// Open file.
	FILE* file = fopen("input.txt", "rb");
	if (file == NULL) {
		fprintf(stderr, "Failed to open file.");
		return 1;
	}
	// File variables.
	size_t total = 0, readIndex = 0;
	bool eof = false;
	char readBuffer[100];

	// Interpretation variables.
	enum ReadState readState;
	size_t winningNumberCount, * winningNumbers = nullptr;

	// Interpret file contents.
	while (!eof) {
		if (feof(file)) {
			// Handle end of file.
			eof = true;
			readState = OwnedNumbers;
		} else {
			// Read character.
			fread(readBuffer + readIndex, sizeof(*readBuffer), 1, file);
			const char c = readBuffer[readIndex];

			if (c == ':') {
				readState = GameID;
			} else if (c == '|') {
				readState = WinningNumbers;
			} else if (c == '\n') {
				readState = OwnedNumbers;
			} else {
				// Trim spaces from start.
				if (readIndex > 0 || c != ' ')
					// Increment index.
					readIndex++;
				continue;
			}
		}
		readBuffer[readIndex] = '\0';

		// Process inputs.
		switch (readState) {
			case WinningNumbers: {
				// Create a copy of the data for tokenization.
				char* const countTemp = malloc((strlen(readBuffer) + 1) * sizeof(char));
				strcpy(countTemp, readBuffer);
				// Get winning number count.
				winningNumberCount = 0;
				if (strtok(countTemp, " ") != nullptr) {
					winningNumberCount++;
					while (strtok(nullptr, " ") != nullptr)
						winningNumberCount++;
				}
				// Cleanup.
				free(countTemp);

				// Create storage.
				if (winningNumbers != nullptr) free(winningNumbers);
				if (winningNumberCount <= 0) {
					// Handle error.
					winningNumbers = nullptr;
					fprintf(stderr, "Failed to find winning numbers.\n");
					break;
				}
				winningNumbers = calloc(winningNumberCount, sizeof(size_t));

				// Read in values.
				size_t valueIndex = 0;
				const char* valueStr = strtok(readBuffer, " ");
				while (valueStr != nullptr && valueIndex < winningNumberCount) {
					// Read value.
					sscanf(valueStr, "%zu", winningNumbers + valueIndex);
					// Increment.
					valueStr = strtok(nullptr, " ");
					valueIndex++;
				}
			} break;
			case OwnedNumbers: {
				// Make sure winning numbers exist.
				if (winningNumberCount <= 0 || winningNumbers == nullptr) break;

				// Data.
				size_t currentScore = 0;

				// Read + check each value.
				const char* valueStr = strtok(readBuffer, " ");
				while (valueStr != nullptr) {
					// Read value.
					size_t value;
					sscanf(valueStr, "%zu", &value);

					// Try to find in winning card.
					for (size_t i = 0; i < winningNumberCount; i++) {
						// Check if current number is winning.
						if (winningNumbers[i] != value) continue;

						// Add to score.
						if (currentScore > 0) currentScore *= 2;
						else currentScore = 1;

						// No more checks need for this value.
						break;
					}

					// Increment.
					valueStr = strtok(nullptr, " ");
				}

				// Add to total.
				total += currentScore;
			} break;
		}

		// Reset index.
		readIndex = 0;
	}


	// Output.
	printf("Output: %zu\n", total);

	// Cleanup.
	fclose(file);
	return 0;
}