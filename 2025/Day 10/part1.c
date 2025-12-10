#include <common/common.h>

#define LIGHTING_MAX 256
#define LIGHTING_BUTTON_MAX 7
#define LIGHTING_STATE_MAX 0b1llu << 10

struct ButtonWiring {
	int* indices;
	int indexCount;
};
struct Lighting {
	uint64_t state;
	uint64_t stateLen;
	struct ButtonWiring* buttons;
	int buttonCount;
} lighting[LIGHTING_MAX];
size_t lightingCount = 0;

struct {
	int bestDepth;
} cache[LIGHTING_STATE_MAX];

void clearState() {
	for (uint64_t i = 0; i < LIGHTING_STATE_MAX; i++)
		cache[i].bestDepth = -1;
}

// when im in x state and i press y button, which z state do i get to...
void minStepsForLighting(int index, uint64_t currentState, int depth) {
	if (cache[currentState].bestDepth != -1 && cache[currentState].bestDepth <= depth)
		return;
	cache[currentState].bestDepth = depth;

	for (int i = 0; i < lighting[index].buttonCount; i++) {
		uint64_t state = currentState;
		for (int j = 0; j < lighting[index].buttons[i].indexCount; j++) {
			const uint64_t k = lighting[index].buttons[i].indices[j];
			state ^= 0b1llu << k;
		}
		// Recurse.
		minStepsForLighting(index, state, depth + 1);
	}
}

int main() {
	FileReader fileReader = FileReader_getInput();

	while (!fileReader.endReached) {
		FileReader_readUntilDelim(&fileReader, "[");
		FileReader_readUntilDelim(&fileReader, "]");

		struct Lighting* targetLighting = &lighting[lightingCount++];
		targetLighting->state = 0;
		targetLighting->stateLen = strlen(fileReader.readBuffer);
		ASSERT(0b1llu << targetLighting->stateLen <= LIGHTING_STATE_MAX, "Oh increase that number");
		for (int i = 0; i < targetLighting->stateLen; i++) {
			targetLighting->state <<= 1;
			if (fileReader.readBuffer[targetLighting->stateLen - (i + 1)] == '.')
				continue;
			targetLighting->state |= 0b1;
		}
		printf("%s => %llu %llx\n", fileReader.readBuffer, targetLighting->state, targetLighting->state);

		targetLighting->buttonCount = 0;

		bool dataToRead = true;
		while (dataToRead) {
			FileReader_readUntilDelim(&fileReader, ")}\n");
			dataToRead = strlen(fileReader.readBuffer) != 0;
			if (!dataToRead)
				continue;

			switch (fileReader.readBuffer[1]) {
				case '(': {
					targetLighting->buttonCount++;
					targetLighting->buttons = realloc(targetLighting->buttons, targetLighting->buttonCount * sizeof(*targetLighting->buttons));
					struct ButtonWiring* wiring = &targetLighting->buttons[targetLighting->buttonCount - 1];
					wiring->indices = nullptr;
					wiring->indexCount = 0;

					char* buffer = fileReader.readBuffer + 2;
					int value = 0;
					while (*buffer != '\0' && sscanf(buffer, "%i,", &value) != 0) {
						while (*buffer != ',' && *buffer != '\0')
							buffer++;
						if (*buffer == ',')
							buffer++;

						wiring->indexCount++;
						wiring->indices = realloc(wiring->indices, wiring->indexCount * sizeof(*wiring->indices));
						wiring->indices[wiring->indexCount - 1] = value;
					}
				} break;
			}
		}
	}

	int totalSteps = 0;
	for (int i = 0; i < lightingCount; i++) {
		clearState();
		minStepsForLighting(i, 0, 0);
		totalSteps += cache[lighting[i].state].bestDepth;
		printf("%i: %i\n", i, cache[lighting[i].state].bestDepth);
	}
	printf("Total steps: %i\n", totalSteps);
}