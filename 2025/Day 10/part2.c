#include <common/common.h>

#define LIGHTING_MAX 256
#define LIGHTING_BUTTON_MAX 15
#define LIGHTING_JOLTAGE_MAX 10

struct ButtonWiring {
	int* indices;
	int indexCount;
};
struct Joltage {
	int joltage[LIGHTING_JOLTAGE_MAX];
};
struct Lighting {
	struct Joltage targetJoltage;
	uint64_t stateLen;
	struct ButtonWiring* buttons;
	int buttonCount;
} lighting[LIGHTING_MAX];
size_t lightingCount = 0;

const char* getName(int val) {
	static char name[6];
	name[0] = 'x';
	int i;
	for (i = 1; i < 5 && (i < 2 || val > 0); i++) {
		name[i] = 'a' + (char)(val % 26);
		val /= 26;
	}
	name[i] = '\0';
	return name;
}
const char* getOutputName(int val) {
	static char name[6];
	name[0] = 'o';
	int i;
	for (i = 1; i < 5 && (i < 2 || val > 0); i++) {
		name[i] = 'a' + (char)(val % 26);
		val /= 26;
	}
	name[i] = '\0';
	return name;
}

int main() {
	FileReader fileReader = FileReader_getInput();

	while (!fileReader.endReached) {
		FileReader_readUntilDelim(&fileReader, "[");
		FileReader_readUntilDelim(&fileReader, "]");

		struct Lighting* targetLighting = &lighting[lightingCount++];
		targetLighting->stateLen = strlen(fileReader.readBuffer);
		for (int i = 0; i < LIGHTING_JOLTAGE_MAX; i++)
			targetLighting->targetJoltage.joltage[i] = 0;
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
					ASSERT(targetLighting->buttonCount <= LIGHTING_BUTTON_MAX, "more buttons please");
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
				case '{': {
					char* buffer = fileReader.readBuffer + 2;
					int value = 0;
					int count = 0;
					while (*buffer != '\0' && sscanf(buffer, "%i,", &value) != 0) {
						while (*buffer != ',' && *buffer != '\0')
							buffer++;
						if (*buffer == ',')
							buffer++;
						targetLighting->targetJoltage.joltage[count++] = value;
					}
				} break;
			}
		}
	}

	char lastFound[6];
	int depth;

	const int splitSize = 45;
	const int splitCount = (lightingCount / splitSize) + (lightingCount % splitSize > 0 ? 1 : 0);
	for (int split = 0; split < splitCount; split++) {
		int variableCounter = 0, outputCounter = 0;

		char filename[20];
		sprintf(filename, "output%i.txt", split + 1);
		FILE* file = fopen(filename, "wb");

		for (int n = 0; n < lightingCount - (splitSize * split) && n < splitSize; n++) {
			struct Lighting* light = &lighting[n + (splitSize * split)];

			int variableStart = variableCounter;
			for (int i = 0; i < light->buttonCount; i++) {
				const char* name = getName(variableStart + i);
				fprintf(file, "(declare-const %s Int)\n", name);
				fprintf(file, "(assert (>= %s 0))\n", name);
				variableCounter++;
			}

			for (int i = 0; i < light->stateLen; i++) {
				lastFound[0] = '\0';
				depth = 0;

				for (int j = 0; j < light->buttonCount; j++) {
					for (int k = 0; k < light->buttons[j].indexCount; k++) {
						if (light->buttons[j].indices[k] != i)
							continue;

						if (lastFound[0] == '\0') {
							fprintf(file, "(assert (= %i ", light->targetJoltage.joltage[i]);
						} else {
							fprintf(file, "(+ %s ", lastFound);
							depth++;
						}
						strcpy(lastFound, getName(variableStart + j));
					}
				}
				if (lastFound[0] == '\0')
					continue;

				fprintf(file, "%s", lastFound);
				for (int j = 0; j < depth; j++) {
					fprintf(file, ")");
				}
				fprintf(file, "))\n");
			}

			{
				const int outputIndex = outputCounter++;
				fprintf(file, "(declare-const %s Int)\n", getOutputName(outputIndex));
				lastFound[0] = '\0';
				depth = 0;

				for (int j = 0; j < light->buttonCount; j++) {
					if (lastFound[0] == '\0') {
						fprintf(file, "(assert (= %s ", getOutputName(outputIndex));
					} else {
						fprintf(file, "(+ %s ", lastFound);
						depth++;
					}
					strcpy(lastFound, getName(variableStart + j));
				}
				fprintf(file, "%s", lastFound);
				for (int j = 0; j < depth; j++) {
					fprintf(file, ")");
				}
				fprintf(file, "))\n");
				fprintf(file, "(minimize %s)\n", getOutputName(outputIndex));
			}
		}

		{
			fprintf(file, "(declare-const output Int)\n");
			lastFound[0] = '\0';
			depth = 0;

			for (int i = 0; i < outputCounter; i++) {
				if (lastFound[0] == '\0') {
					fprintf(file, "(assert (= output ");
				} else {
					fprintf(file, "(+ %s ", lastFound);
					depth++;
				}
				strcpy(lastFound, getOutputName(i));
			}
			fprintf(file, "%s", lastFound);
			for (int j = 0; j < depth; j++) {
				fprintf(file, ")");
			}
			fprintf(file, "))\n");
		}
		fprintf(file, "(check-sat)\n(get-model)\n");
		fclose(file);
	}

	printf(">> Enter this into \"https://microsoft.github.io/z3guide/playground/Freeform%%20Editing\"\n");
	printf("yes you have to do each one manually.\n");
	printf("no i did not enjoy this one lmao.\n");
}