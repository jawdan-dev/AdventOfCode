#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#define nullptr NULL

struct MatchValue {
	const char* m_key;
	const size_t m_value;
};

static struct MatchValue s_matchList[] = {
	{ "0", 0 },
	{ "1", 1 },
	{ "2", 2 },
	{ "3", 3 },
	{ "4", 4 },
	{ "5", 5 },
	{ "6", 6 },
	{ "7", 7 },
	{ "8", 8 },
	{ "9", 9 },
	{ "zero", 0 },
	{ "one", 1 },
	{ "two", 2 },
	{ "three", 3 },
	{ "four", 4 },
	{ "five", 5 },
	{ "six", 6 },
	{ "seven", 7 },
	{ "eight", 8 },
	{ "nine", 9 },
};

int main() {
	// Open file.
	FILE* file = fopen("input.txt", "rb");
	if (file == nullptr) {
		fprintf(stderr, "Failed to open file.");
		return 1;
	}

	// Input variables.
	bool found, eofChecked = false, leftFound;
	size_t total = 0, lineIndex = 0, left, right;
	char lineBuffer[100];

	// Read inputs.
	while (!feof(file) || !eofChecked) {
		// Handle end of file.
		if (feof(file))
			eofChecked = true;
		if (!eofChecked) {
			// Read character.
			fread(lineBuffer + lineIndex, sizeof(*lineBuffer), 1, file);

			// Check for new line.
			if (lineBuffer[lineIndex] != '\n') {
				// Increment index.
				lineIndex++;
				continue;
			}
		}
		lineBuffer[lineIndex] = '\0';

		// Find any value matches.
		// NOTE: A left-to-right for left, and a right-to-left for right approach would be better.
		// However, the strings arent long enough for the extra effort.
		leftFound = false;
		for (size_t i = 0; i < lineIndex; i++) {
			for (size_t j = 0; j < sizeof(s_matchList) / sizeof(*s_matchList); j++) {
				// Get match.
				struct MatchValue* match = s_matchList + j;
				// Check string length.
				const size_t matchLength = strlen(match->m_key);
				if (matchLength > lineIndex - i) continue;

				// Check buffer for match.
				found = true;
				for (size_t k = 0; k < matchLength && (i + k) < lineIndex && found; k++) {
					// Check for match.
					if (lineBuffer[i + k] != match->m_key[k])
						found = false;
				}
				// Update value.
				if (found) {
					const size_t value = match->m_value;

					if (!leftFound) {
						// Update left value.
						left = value;
						leftFound = true;
					}

					// Update right value.
					right = value;
				}
			}
		}

		// Add to total.
		if (leftFound) total += (left * 10) + right;

		// Update details.
		lineIndex = 0;
	}

	// Output.
	printf("%zu\n", total);

	return 0;
}