#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#define nullptr NULL

struct Number {
	char* m_rawData;
	size_t m_rawX, m_rawY, m_length, m_value;
};

const bool isNumberSymbol(const char c);
const bool isSymbol(const char c);

const size_t getAdjacentRatio(
	char* const schematic, const size_t schematicWidth, const size_t schematicHeight,
	struct Number* number
);

struct Number getNumber(
	char* const schematic, const size_t schematicWidth, const size_t schematicHeight,
	const size_t numberX, const size_t numberY
);
void clearNumber(struct Number number);


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
			const size_t index = i + (j * schematicWidth);
			if (!isNumberSymbol(schematic[index])) continue;

			struct Number number = getNumber(
				schematic, schematicWidth, schematicHeight,
				i, j
			);
			// Remove number from schematic.
			clearNumber(number);

			// Get gear ratio.
			const size_t ratio = getAdjacentRatio(
				schematic, schematicWidth, schematicHeight,
				&number
			);

			printf("Found number %zu with ratio %zu\n", number.m_value, ratio);

			// Add to total.
			total += ratio * number.m_value;
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

const size_t getAdjacentRatio(
	char* const schematic, const size_t schematicWidth, const size_t schematicHeight,
	struct Number* number
) {
	// Get bounds.
	const size_t
		minX = number->m_rawX > 0 ? number->m_rawX - 1 : 0,
		endX = number->m_rawX + number->m_length,
		maxX = endX + (endX + 1 < schematicWidth ? 1 : 0),
		minY = number->m_rawY > 0 ? number->m_rawY - 1 : 0,
		endY = number->m_rawY + 1,
		maxY = endY + (endY + 1 < schematicHeight ? 1 : 0);

	// Read bounds.
	for (size_t y = minY; y < maxY; y++) {
		for (size_t x = minX; x < maxX; x++) {
			// Make sure index is outside.
			if (y == number->m_rawY && (x >= number->m_rawX && x < endX)) continue;

			const size_t index = x + (y * schematicWidth);

			// Check if a possible gear.
			if (schematic[index] == '*') {
				const size_t
					inMinX = x > 0 ? x - 1 : 0,
					inMaxX = x + 1 < schematicWidth ? x + 1 : x,
					inMinY = y > 0 ? y - 1 : 0,
					inMaxY = y + 1 < schematicHeight ? y + 1 : y;

				// Look for other numbers.
				size_t foundNumber = 0;
				struct Number ratioNumber;

				for (size_t inY = inMinY; inY <= inMaxY; inY++) {
					for (size_t inX = inMinX; inX <= inMaxX; inX++) {
						// Check if number found.
						if (isNumberSymbol(schematic[inX + (inY * schematicWidth)])) {
							ratioNumber = getNumber(
								schematic, schematicWidth, schematicHeight,
								inX, inY
							);
							clearNumber(ratioNumber);
							foundNumber++;
						}
					}
				}
				// Return ratio if exactly one other is found.
				if (foundNumber == 1) return ratioNumber.m_value;
			}
		}
	}

	// Ratio not found.
	return 0;
}

struct Number getNumber(
	char* const schematic, const size_t schematicWidth, const size_t schematicHeight,
	const size_t numberX, const size_t numberY
) {
	struct Number number;
	number.m_rawX = numberX;
	number.m_rawY = numberY;

	// Make sure we're at the start.
	while (number.m_rawX > 0 && isNumberSymbol(schematic[(number.m_rawX - 1) + (number.m_rawY * schematicWidth)])) number.m_rawX--;

	// Return new ptr.
	number.m_rawData = schematic + (number.m_rawX + (number.m_rawY * schematicWidth));

	// Find end.
	number.m_value = 0;
	number.m_length = 0;
	while (
		number.m_rawX + number.m_length < schematicWidth &&
		isNumberSymbol(schematic[(number.m_rawX + number.m_length) + (number.m_rawY * schematicWidth)])) {
		// Calculate value.
		number.m_value *= 10;
		number.m_value += number.m_rawData[number.m_length] - '0';
		// Update length.
		number.m_length++;
	}

	return number;
}
void clearNumber(struct Number number) {
	for (size_t i = 0; i < number.m_length; i++) number.m_rawData[i] = '.';
}