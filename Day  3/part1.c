#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#define nullptr NULL

const bool isNumberSymbol(const char c);
const bool isSymbol(const char c);

const size_t getGear(
	const char* const schematic, const size_t schematicWidth, const size_t schematicHeight,
	const size_t stringX, const size_t stringY
);
const bool getAnyAdjacent(
	const char* const schematic, const size_t schematicWidth, const size_t schematicHeight,
	const size_t stringX, const size_t stringY, const size_t stringLength
);
const size_t getNumberFromString(const char* const string, const size_t stringLength);


int main() {
	// Open file.
	FILE* file = fopen("input.txt", "rb");
	if (file == NULL) {
		fprintf(stderr, "Failed to open file.\n");
		return 1;
	}

	// File variables.
	char c;
	size_t total = 0;

	// Calculate schematic width.
	size_t schematicWidth = 0;
	while (fread(&c, sizeof(c), 1, file) > 0 && c != '\n') schematicWidth++;
	fseek(file, 0, SEEK_SET);
	// Calculate schematic height.
	fseek(file, 0, SEEK_END);
	long fileSize = ftell(file);
	// (filesize + missing \n) / (line width exc.\n + \n);
	const size_t schematicHeight = (fileSize + 1) / (schematicWidth + 1);
	fseek(file, 0, SEEK_SET);

	// Make sure schematic dimensiosn are valid.
	if (schematicWidth <= 0 || schematicHeight <= 0) {
		fprintf(stderr, "Failed to get engine schematic dimensions.\n");
		fclose(file);
		return 1;
	}
	printf("Engine Schematic dimensions: %zu x %zu\n", schematicWidth, schematicHeight);

	// Allocate storage.
	char* const schematic = malloc(sizeof(char) * schematicWidth * schematicHeight);

	// Read schematic.
	for (size_t j = 0; j < schematicHeight; j++) {
		// Read line.
		fread(schematic + (j * schematicWidth), sizeof(c), schematicWidth, file);
		// Read newline / eof.
		fread(&c, sizeof(c), 1, file);
	}

	// Process schematic.
	for (size_t j = 0; j < schematicHeight; j++) {
		for (size_t i = 0; i < schematicWidth; i++) {
			// Get start of number.
			const size_t startIndex = i + (j * schematicWidth);
			if (!isNumberSymbol(schematic[startIndex])) continue;

			// Get length of number.
			while (i < schematicWidth && isNumberSymbol(schematic[i + (j * schematicWidth)])) i++;
			const size_t
				endIndex = i + (j * schematicWidth),
				numberLength = endIndex - startIndex;

			// Get if valid.
			if (getAnyAdjacent(
				schematic, schematicWidth, schematicHeight,
				i - numberLength, j, numberLength
			)) {
				// Add to total.
				total += getNumberFromString(schematic + startIndex, numberLength);
			}
		}
	}

	// Output.
	printf("Output: %zu\n", total);

	// Cleanup.
	free(schematic);
	fclose(file);
	return 0;
}

const bool isNumberSymbol(const char c) {
	return c >= '0' && c <= '9';
}
const bool isSymbol(const char c) {
	return c != '.' && !isNumberSymbol(c);
}

const bool getAnyAdjacent(
	const char* const schematic, const size_t schematicWidth, const size_t schematicHeight,
	const size_t stringX, const size_t stringY, const size_t stringLength
) {
	// Get bounds.
	const size_t
		minX = stringX > 0 ? stringX - 1 : 0,
		endX = stringX + stringLength,
		maxX = endX + (endX + 1 < schematicWidth ? 1 : 0),
		minY = stringY > 0 ? stringY - 1 : 0,
		endY = stringY + 1,
		maxY = endY + (endY + 1 < schematicHeight ? 1 : 0);

	// Read bounds.
	for (size_t y = minY; y < maxY; y++) {
		for (size_t x = minX; x < maxX; x++) {
			// Make sure index is outside.
			if (y == stringY && (x >= stringX && x < endX)) continue;

			// Check if symbol.
			if (isSymbol(schematic[x + (y * schematicWidth)]))
				return true;
		}
	}

	// Symbol not found.
	return false;
}
const size_t getNumberFromString(const char* const string, const size_t stringLength) {
	// NOTE: this does not handle non-numeric characters well.
	size_t value = 0;
	for (size_t i = 0; i < stringLength; i++) {
		value *= 10;
		value += string[i] - '0';
	}
	return value;
}