#include <common/common.h>
#include <common/point.h>

#define MAP_MAX 50
static char map[MAP_MAX][MAP_MAX];
static int width = 0, height = 0;

#define ANTENNA_MAX 32
#define ANTENNA_CHARS 256
static Point antennaPositions[ANTENNA_CHARS][ANTENNA_MAX];
static int antennaCounts[ANTENNA_CHARS];

int main() {
	// Read in map.
	FileReader fileReader = FileReader_getInput();
	FileReader_readMap(&fileReader, (char*)map, MAP_MAX, MAP_MAX, &width, &height);

	// Setup antenna/antinodes storage and get all antenna positions.
	char c;
	for (int i = 0; i < ANTENNA_CHARS; i++) antennaCounts[i] = 0;
	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++)
			if ((c = map[i][j]) != '.')
				antennaPositions[c][antennaCounts[c]++] = (Point) { .x = i, .y = j };

	// For all characters,
	for (int c = 0; c < ANTENNA_CHARS; c++) {
		// Cartesian node compare.
		for (int i = 0; i < antennaCounts[c]; i++) {
			for (int j = i + 1; j < antennaCounts[c]; j++) {
				// Get comparison antennas.
				const Point p1 = antennaPositions[c][i], p2 = antennaPositions[c][j];
				// Get antinode check positions.
				const Point change = Point_subtract(&p2, &p1);
				const Point checks[] = { Point_subtract(&p1, &change), Point_add(&p2, &change) };
				// Check points.
				for (int k = 0; k < ARRAY_SIZE(checks); k++) {
					const Point checkPoint = checks[k];
					if (checkPoint.x < 0 || checkPoint.x >= width ||
						checkPoint.y < 0 || checkPoint.y >= height) continue;
					// Antinode found.
					map[checkPoint.x][checkPoint.y] = '#';
				}
			}
		}
	}

	// Total up antinodes.
	int totalAntinodes = 0;
	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++)
			totalAntinodes += map[i][j] == '#';

	// Debug (wanted to see how small it could get).
	for (int j = 0; j < height; j++, printf("\n")) for (int i = 0; i < width; printf("%c", map[i][j]), i++);

	// Output count.
	printf("Total antinodes: %i\n", totalAntinodes);
}