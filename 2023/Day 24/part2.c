// Load FileBuffer.
#define FILEBUFFER_READBUFFERSIZE 100
enum ReadState {
	ReadState_Line,
};
#include "../Common/FileBuffer.h"

#define HAIL_MAX 300

typedef int64_t PrecisionType;

typedef struct vec3 {
	PrecisionType x, y, z;
} vec3;

typedef struct Hail {
	vec3 pos, vel;
} Hail;

// Entry point.
int main() {
	// Create file buffer.
	struct FileBuffer fileBuffer;
	FileBuffer_OpenFile(&fileBuffer, "input.txt");

	// File delims.
	struct FileDelim delims[] = {
		{ '\n', ReadState_Line }
	};
	FileBuffer_SetDelims(&fileBuffer, delims, sizeof(delims) / sizeof(*delims), ReadState_Line);

	// Interpretation variables.
	Hail hail[HAIL_MAX];
	size_t hailCount = 0;

	// Interpret file.
	while (FileBuffer_ReadUntilNextDelim(&fileBuffer)) {
		// Processing.
		switch (fileBuffer.readState) {
			case ReadState_Line: {
				PrecisionType values[6];
				size_t valuesRead = 0;

				// Read values.
				char* valueStr = strtok(fileBuffer.readBuffer, " ,@");
				while (valueStr != nullptr) {
					// Read data.
					sscanf(valueStr, "%lli", &values[valuesRead++]);

					// Increment.
					valueStr = strtok(nullptr, " ,@");
				}

				if (valuesRead != 6) {
					fprintf(stderr, "Failed to read hail fully.\n");
					abort();
				}
				if (hailCount >= HAIL_MAX) {
					fprintf(stderr, "Hail max exceeded\n");
					abort();
				}

				hail[hailCount++] = (Hail) {
					.pos = (vec3) {
						.x = values[0],
						.y = values[1],
						.z = values[2],
					},
					.vel = (vec3) {
						.x = values[3],
						.y = values[4],
						.z = values[5],
					},
				};
			} break;
		}
	}

	FILE* const f = fopen("z3.txt", "w");

#	define FBUFFER_MAX 2048
	char fBuffer[FBUFFER_MAX];

#define WRITETOFILE(...) \
		snprintf(fBuffer, FBUFFER_MAX, __VA_ARGS__); \
		fwrite(fBuffer, sizeof(*fBuffer), strlen(fBuffer), f);

	WRITETOFILE("\n(echo \"Starting z3.\")\n");
	WRITETOFILE("(declare-const x (_ BitVec 64))\n");
	WRITETOFILE("(declare-const y (_ BitVec 64))\n");
	WRITETOFILE("(declare-const z (_ BitVec 64))\n");
	WRITETOFILE("(declare-const vx (_ BitVec 64))\n");
	WRITETOFILE("(declare-const vy (_ BitVec 64))\n");
	WRITETOFILE("(declare-const vz (_ BitVec 64))\n");
	for (size_t i = 0; i < hailCount; i++) {
	}

	WRITETOFILE("\n");
	// TODO: IF THE WRONG ANSWER IS GOTTEN INCREASE FROM 30 (this is mainly for the html performance).
	for (size_t i = 0; i < hailCount && i < 30; i++) {
		Hail* const h = &hail[i];
		const size_t label = i + 1;
		WRITETOFILE("\n(echo \"Asserting %zu:\")\n", label);
		WRITETOFILE("(declare-const t%zu (_ BitVec 64))\n", label);
		WRITETOFILE("(assert (bvugt t%zu #x0000000000000000))\n", label);
		WRITETOFILE("(assert (= (bvadd x (bvmul vx t%zu)) (bvadd #x%.16llx (bvmul #x%.16llx t%zu))))\n", label, h->pos.x, h->vel.x, label);
		WRITETOFILE("(assert (= (bvadd y (bvmul vy t%zu)) (bvadd #x%.16llx (bvmul #x%.16llx t%zu))))\n", label, h->pos.y, h->vel.y, label);
		WRITETOFILE("(assert (= (bvadd z (bvmul vz t%zu)) (bvadd #x%.16llx (bvmul #x%.16llx t%zu))))\n", label, h->pos.z, h->vel.z, label);
	}
	WRITETOFILE("\n(check-sat)\n");
	WRITETOFILE("\n(echo \"Output:\")\n");
	WRITETOFILE("(eval x)\n(eval y)\n(eval z)\n");

	fclose(f);

	printf("--------------------------\n");
	printf("1. Enter the code from the file \"z3.txt\" into https://jfmc.github.io/z3-play/.\n");
	printf("2. Run.\n");
	printf("3. Place the results here:\n");
	printf("--------------------------\n");
	PrecisionType x, y, z;
	scanf("#x%llx #x%llx #x%llx", &x, &y, &z);
	printf("--------------------------\n");
	printf("Output: %lli\n", x + y + z);
	printf("--------------------------\n");

	return 0;
}