#include <common/common.h>

#define MAP_SIZE 8
static char map[MAP_SIZE][MAP_SIZE];
static int width = 0, height = 0;
//
#define KEYLOCK_WIDTH 5
#define LOCKS_MAX 500
static int locks[LOCKS_MAX][KEYLOCK_WIDTH];
int lockCount = 0;
#define KEYS_MAX 500
static int keys[KEYS_MAX][KEYLOCK_WIDTH];
int keyCount = 0;

int main() {
	FileReader fileReader = FileReader_getInput();
	while (!fileReader.endReached) {
		FileReader_readMap(&fileReader, (char*)&map, MAP_SIZE, MAP_SIZE, &width, &height);
		//
		bool isKey = true;
		for (int i = 0; i < width && isKey; i++)
			if (map[i][0] == '#')
				isKey = false;
		//
		if (isKey) {
			for (int i = 0; i < width; i++) {
				int j;
				for (j = height - 2; j >= 0; j--)
					if (map[i][j] != '#')
						break;
				keys[keyCount][i] = (height - 2) - j;
			}
			keyCount++;
		} else {
			for (int i = 0; i < width; i++) {
				int j;
				for (j = height - 1; j >= 0; j--)
					if (map[i][j] != '.')
						break;
				locks[lockCount][i] = (height - 2) - j;
			}
			lockCount++;
		}
	}
	//
	int totalUniquePairs = 0;
	for (int i = 0; i < keyCount; i++) {
		for (int j = 0; j < lockCount; j++) {
			bool goodPair = true;
			for (int k = 0; k < KEYLOCK_WIDTH && goodPair; k++) 
				if (keys[i][k] > locks[j][k])
					goodPair = false;
			if (goodPair) totalUniquePairs++;
		}
	}
	//
	printf("Total unique keylock pairs: %i\n", totalUniquePairs);
}