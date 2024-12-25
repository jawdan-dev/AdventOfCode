#include <common/common.h>
#include <common/point.h>

#define MAP_MAX 1000
int map[MAP_MAX][MAP_MAX];

int main() {
	for (int i = 0; i < MAP_MAX; i++)
		for (int j = 0; j < MAP_MAX; j++)
			map[i][j] = 0;
	//
	FileReader fileReader = FileReader_getInput();
	while (!fileReader.endReached) {
		FileReader_readUntilDelim(&fileReader, "\n");
		//
		char command[10];
		Point min, max;
		if (fileReader.readBuffer[1] == 'u')
			sscanf(fileReader.readBuffer, "turn %s %i,%i through %i,%i", command, &min.x, &min.y, &max.x, &max.y);
		else
			sscanf(fileReader.readBuffer, "%s %i,%i through %i,%i", command, &min.x, &min.y, &max.x, &max.y);
		//
		for (int i = min.x; i <= max.x; i++) {
			for (int j = min.y; j <= max.y; j++) {
				switch (command[1]) {
					case 'o': map[i][j] += 2; break; // Toggle.
					case 'n': map[i][j] += 1; break; // Turn on.
					case 'f': map[i][j] = max(0, map[i][j] - 1); break; // Turn off.
				}
			}
		}
	}
	//
	SafeType totalBrightness = 0;
	for (int i = 0; i < MAP_MAX; i++)
		for (int j = 0; j < MAP_MAX; j++)
			totalBrightness += map[i][j];
	printf("Total brightness: %lli\n", totalBrightness);
}