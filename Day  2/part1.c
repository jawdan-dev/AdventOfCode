#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#define nullptr NULL

struct GameInformation {
	size_t m_gameId;
	union {
		struct {
			size_t
				m_maxRedCount,
				m_maxGreenCount,
				m_maxBlueCount;
		};
		size_t m_maxColorCounts[3];
	};
};
enum ReadType {
	GameID, GameInformation, GameInformationEnd
};
enum Color {
	Red, Green, Blue
};

int main() {
	// Open file.
	FILE* file = fopen("input.txt", "rb");
	if (file == NULL) {
		fprintf(stderr, "Failed to open file.");
		return 1;
	}

	// Config values.
	const size_t
		c_minRedCount = 12,
		c_minGreenCount = 13,
		c_minBlueCount = 14;

	// Input variables.
	bool eofChecked = false;
	size_t total = 0, readIndex = 0;
	char lineBuffer[100];
	enum ReadType lineType;

	// Initialize game.
	struct GameInformation currentGame;
	currentGame.m_gameId = 0;
	currentGame.m_maxRedCount = 0;
	currentGame.m_maxGreenCount = 0;
	currentGame.m_maxBlueCount = 0;

	// Read inputs.
	while (!feof(file) || !eofChecked) {
		// Handle end of file.
		if (feof(file)) {
			eofChecked = true;
			lineType = GameInformationEnd;
		} else {
			// Read character.
			fread(lineBuffer + readIndex, sizeof(*lineBuffer), 1, file);
			const char c = lineBuffer[readIndex];

			// Check for delims.
			if (c == ':') {
				lineType = GameID;
			} else if (c == ',' || c == ';') {
				lineType = GameInformation;
			} else if (c == '\n') {
				lineType = GameInformationEnd;
			} else {
				// Trim first spaces.
				if (readIndex > 0 || c != ' ')
					// Not a delim -> Line data.
					readIndex++;

				// No need to process line yet.
				continue;
			}
		}
		// Make sure we've got that
		lineBuffer[readIndex] = '\0';

		// Process line.
		switch (lineType) {
			case GameID: {
				// Read id.
				const char* const idPtr = lineBuffer + (sizeof("Game ") / sizeof(char)) - 1;
				int id = atoi(idPtr);

				// Update game information.
				currentGame.m_gameId = id;
			} break;

			case GameInformation:
			case GameInformationEnd: {
				// Read value.
				const char* valuePtr = strtok(lineBuffer, " ");
				const int value = atoi(valuePtr);

				// Read color.
				const char* const colorPtr = strtok(nullptr, " ");
				enum Color color;
				if (strcmp(colorPtr, "red") == 0) color = Red;
				else if (strcmp(colorPtr, "green") == 0) color = Green;
				else if (strcmp(colorPtr, "blue") == 0) color = Blue;
				else break;

				// Add to game.
				size_t* maxColorData = &currentGame.m_maxColorCounts[color];
				if (*maxColorData < value)
					*maxColorData = value;
			} break;
		}

		if (lineType == GameInformationEnd) {
			// Check if current game is valid.
			if (currentGame.m_maxRedCount <= c_minRedCount &&
				currentGame.m_maxGreenCount <= c_minGreenCount &&
				currentGame.m_maxBlueCount <= c_minBlueCount) {
				// Add to total.
				total += currentGame.m_gameId;
			}

			// Reset game.
			currentGame.m_gameId = 0;
			currentGame.m_maxRedCount = 0;
			currentGame.m_maxGreenCount = 0;
			currentGame.m_maxBlueCount = 0;
		}

		// Update details.
		readIndex = 0;
	}

	// Output.
	printf("%zu\n", total);

	return 0;
}