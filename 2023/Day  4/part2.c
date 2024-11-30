#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#define nullptr NULL

struct Card {
	size_t m_cardId, m_winningCount;
};
const size_t getCardCountRecursive(const struct Card* const cardList);

int main() {
	// Open file.
	FILE* file = fopen("input.txt", "rb");
	if (file == NULL) {
		fprintf(stderr, "Failed to open file.");
		return 1;
	}
	// File variables.
	size_t readIndex = 0;
	bool eof = false;
	char readBuffer[100];

	// Interpretation variables.
	enum ReadState {
		GameID,
		WinningNumbers, OwnedNumbers
	} readState;

	// Fixed size for ease-of-programming.
	struct Card cardList[500];
	size_t firstWinningCard = 0, cardCount = 0;

	// Current card data.
	size_t
		cardId = 0,
		winningNumberCount, * winningNumbers = nullptr;

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
			case GameID: {
				if (sscanf(readBuffer, "Card %zu", &cardId) <= 0) {
					fprintf(stderr, "Failed to read cardID.\n");
					cardId = 0;
					break;
				}
			} break;

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
				size_t ownedWinnerCount = 0;

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

						// Winner found!
						ownedWinnerCount++;

						// No more checks need for this value.
						break;
					}

					// Increment.
					valueStr = strtok(nullptr, " ");
				}

				// Check if first winning card.
				if (firstWinningCard <= 0 && ownedWinnerCount > 0)
					firstWinningCard = cardId;

				// Add to list.
				cardList[cardCount].m_cardId = cardId;
				cardList[cardCount].m_winningCount = ownedWinnerCount;

				// Increment card count.
				cardCount++;
			} break;
		}

		// Reset index.
		readIndex = 0;
	}

	// Calculate total scratch cards.
	size_t total = 0;
	for (size_t i = 0; i < cardCount; i++) {
		total += getCardCountRecursive(cardList + i);
	}

	// Output.
	printf("Output: %zu\n", total);

	// Cleanup.
	fclose(file);
	return 0;
}

const size_t getCardCountRecursive(const struct Card* const cardList) {
	size_t cardCount = 1;
	for (size_t i = 0; i < cardList->m_winningCount; i++) {
		cardCount += getCardCountRecursive(cardList + i + 1);
	}
	return cardCount;
}