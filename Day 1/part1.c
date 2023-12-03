#include <stdio.h>
#include <stdbool.h>

int main() {
	// Open file.
	FILE* file = fopen("input.txt", "rb");
	if (file == NULL) {
		fprintf(stderr, "Failed to open file.");
		return 1;
	}

	// Input variables.
	char c;
	bool newLine = true;
	size_t total = 0, left, right;

	// Read inputs.
	while (!feof(file)) {
		// Read character.
		fread(&c, sizeof(c), 1, file);

		// Check if number.
		if (c >= '0' && c <= '9') {
			// Get number.
			const size_t value = c - '0';

			// Update values.
			if (newLine) left = value;
			right = value;

			// Update details.
			newLine = false;
		} else if (c == '\n') {
			// Add to total.
			if (!newLine)
				total += (left * 10) + right;

			// Update details.
			newLine = true;
		}
	}
	// Add final value to total.
	if (!newLine)
		total += (left * 10) + right;

	// Output.
	printf("%zu\n", total);

	return 0;
}