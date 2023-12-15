#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#define nullptr NULL

enum ReadState {
	ReadState_Heading,
	ReadState_Value,
};

enum TargetList {
	TargetList_SeedToSoil,
	TargetList_SoilToFertilizer,
	TargetList_FertilizerToWater,
	TargetList_WaterToLight,
	TargetList_LightToTemperature,
	TargetList_TemperatureToHumidity,
	TargetList_HumidityToLocation,

	TargetList_Count, // NOTE: Also used as error.

	TargetList_Seeds,
};

struct AlmanacEntry {
	union {
		struct {
			size_t m_desinationStart, m_sourceStart, m_rangeLength;
		};
		size_t m_rawInput[3];
	};
};
struct AlmanacMap {
	struct AlmanacEntry m_entries[64];
	size_t m_entryCount;
};
struct Almanac {
	struct AlmanacMap m_maps[TargetList_Count];
};
const size_t findSeedLocation(const struct Almanac* const almanac, const size_t seed);
const size_t findEndLocation(const struct Almanac* const almanac, const size_t currentMap, const size_t currentValue);


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
	char readBuffer[2000]; // Large lines.
	enum ReadState readState;

	// Interpretation variables.
	enum TargetList targetList = TargetList_Count;
	struct Almanac almanac;
	size_t seeds[100], seedCount = 0;

	// Initialize almanac.
	for (size_t i = 0; i < sizeof(almanac.m_maps) / sizeof(*almanac.m_maps); i++)
		almanac.m_maps[i].m_entryCount = 0;

	// Interpret file contents.
	while (!eof) {
		if (feof(file)) {
			// Handle end of file.
			eof = true;
			readState = ReadState_Value;
		} else {
			// Read character.
			fread(&readBuffer[readIndex], sizeof(*readBuffer), 1, file);
			const char c = readBuffer[readIndex];

			if (c == ':') {
				readState = ReadState_Heading;
			} else if (c == '\n') {
				readState = ReadState_Value;
			} else {
				// Trim spaces from start.
				if (readIndex > 0 || c != ' ')
					// Increment index.
					readIndex++;
				continue;
			}

		}
		// Mark end.
		readBuffer[readIndex] = '\0';
		readIndex = 0;

		// Handle line.
		switch (readState) {
			case ReadState_Heading: {
				// Test for heading type.
				if (strcmp(readBuffer, "seeds") == 0) targetList = TargetList_Seeds;
				else if (strcmp(readBuffer, "seed-to-soil map") == 0) targetList = TargetList_SeedToSoil;
				else if (strcmp(readBuffer, "soil-to-fertilizer map") == 0) targetList = TargetList_SoilToFertilizer;
				else if (strcmp(readBuffer, "fertilizer-to-water map") == 0) targetList = TargetList_FertilizerToWater;
				else if (strcmp(readBuffer, "water-to-light map") == 0) targetList = TargetList_WaterToLight;
				else if (strcmp(readBuffer, "light-to-temperature map") == 0) targetList = TargetList_LightToTemperature;
				else if (strcmp(readBuffer, "temperature-to-humidity map") == 0) targetList = TargetList_TemperatureToHumidity;
				else if (strcmp(readBuffer, "humidity-to-location map") == 0) targetList = TargetList_HumidityToLocation;
				// No heading found.
				else targetList = TargetList_Count;
			} break;
			case ReadState_Value: {
				switch (targetList) {
					case TargetList_Seeds: {
						char* valueStr = strtok(readBuffer, " ");
						while (valueStr != nullptr) {
							// Read value.
							size_t value;
							sscanf(valueStr, "%zu", &value);

							// Insert into seed list.
							seeds[seedCount] = value;
							seedCount++;

							// Increment.
							valueStr = strtok(nullptr, " ");
						}
					} break;

					case TargetList_SeedToSoil:
					case TargetList_SoilToFertilizer:
					case TargetList_FertilizerToWater:
					case TargetList_WaterToLight:
					case TargetList_LightToTemperature:
					case TargetList_TemperatureToHumidity:
					case TargetList_HumidityToLocation: {
						// Get target.
						struct AlmanacMap* const targetMap = almanac.m_maps + targetList;
						size_t values[3], valueCount = 0;

						char* valueStr = strtok(readBuffer, " ");
						while (valueStr != nullptr && valueCount < 3) {
							// Read value.
							sscanf(valueStr, "%zu", values + valueCount);

							// Increment.
							valueStr = strtok(nullptr, " ");
							valueCount++;
						}

						// Error check.
						if (valueCount != 3) break;

						// Add new entry.
						memcpy(
							targetMap->m_entries[targetMap->m_entryCount].m_rawInput,
							values,
							sizeof(values)
						);
						targetMap->m_entryCount++;
					} break;
				}
			} break;
		}
	}


	if (seedCount > 0) {
		size_t closestSeed = findSeedLocation(&almanac, seeds[0]);
		for (size_t i = 1; i < seedCount; i++) {
			const size_t seedLocation = findSeedLocation(&almanac, seeds[i]);
			if (closestSeed > seedLocation)
				closestSeed = seedLocation;
		}

		// Output.
		printf("Output: %zu\n", closestSeed);
	} else {
		fprintf(stderr, "No seeds found to evaluate.\n");
	}

	// Cleanup.
	fclose(file);
	return 0;
}


const size_t findSeedLocation(const struct Almanac* const almanac, const size_t seed) {
	return findEndLocation(almanac, 0, seed);
}
const size_t findEndLocation(const struct Almanac* const almanac, const size_t currentMap, const size_t currentValue) {
	// Exit case.
	if (currentMap >= sizeof(almanac->m_maps) / sizeof(*almanac->m_maps))
		return currentValue;

	// Helper variable.
	const struct AlmanacMap* const map = &almanac->m_maps[currentMap];

	// Default to same value.
	size_t newValue = currentValue;

	// Find more specific case.
	for (size_t i = 0; i < map->m_entryCount; i++) {
		const struct AlmanacEntry* const entry = &map->m_entries[i];

		// Ignore if out of range.
		if (entry->m_sourceStart > newValue || entry->m_sourceStart + entry->m_rangeLength <= newValue) continue;

		// Convert.
		const size_t offset = currentValue - entry->m_sourceStart;
		newValue = entry->m_desinationStart + offset;
		break;
	}

	// Recurse.
	return findEndLocation(almanac, currentMap + 1, newValue);
}