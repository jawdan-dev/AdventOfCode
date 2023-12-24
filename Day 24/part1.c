// Load FileBuffer.
#define FILEBUFFER_READBUFFERSIZE 100
enum ReadState {
	ReadState_Line,
};
#include "../Common/FileBuffer.h"

#include <math.h>

#define HAIL_MAX 300


typedef double PrecisionType;

typedef struct vec3 {
	PrecisionType x, y;
} vec3;

const PrecisionType dot(const vec3 v1, const vec3 v2) {
	return (v1.x * v2.x) + (v1.y * v2.y);
}
const vec3 cross(const vec3 v) {
	return (vec3) {
		.x = v.y, .y = -v.x,
	};
}
const PrecisionType mag(const vec3 v) {
	return sqrt(dot(v, v));
}
const vec3 norm(const vec3 v) {
	const PrecisionType magnitude = mag(v);
	if (magnitude == 0) return v;
	return (vec3) {
		.x = v.x / magnitude, .y = v.y / magnitude,
	};
}


typedef struct Hail {
	vec3 pos, vel;
} Hail;

const vec3 getIntersection(const Hail qHail, const Hail pHail) {
	const vec3 q = qHail.pos, p = pHail.pos;
	const vec3 s = norm(qHail.vel), r = norm(pHail.vel);

	const vec3 qmp = (vec3) { .x = q.x - p.x, .y = q.y - p.y, };

	const PrecisionType
		u = dot(qmp, cross(r)) / dot(r, cross(s)),
		t = dot(qmp, cross(s)) / dot(r, cross(s));

	if (t <= 0 || u <= 0) {
		return (vec3) {
			INFINITY, INFINITY
		};
	}

	//X = P + R * t = Q + S * u
	return (vec3) {
		.x = p.x + (r.x * t),
			.y = p.y + (r.y * t),
	};
}

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
					sscanf(valueStr, "%lf", &values[valuesRead++]);

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
						//.z = values[2],
					},
					.vel = (vec3) {
						.x = values[3],
						.y = values[4],
						//.z = values[5],
					},
				};
			} break;
		}
	}

	const PrecisionType testAreaMin = 200000000000000, testAreaMax = 400000000000000;
	//const PrecisionType testAreaMin = 7, testAreaMax = 27;

	size_t intersectionInBoundsCount = 0;
	for (size_t i = 0; i < hailCount; i++) {
		for (size_t j = i + 1; j < hailCount; j++) {
			const vec3 intersection = getIntersection(hail[i], hail[j]);

			if (intersection.x == INFINITY || intersection.y == INFINITY) continue;

			if (intersection.x < testAreaMin || intersection.x > testAreaMax ||
				intersection.y < testAreaMin || intersection.y > testAreaMax) continue;

			printf("%f %f vs %f %f : { %f %f }\n", hail[i].pos.x, hail[i].pos.y, hail[j].pos.x, hail[j].pos.y, intersection.x, intersection.y);
			intersectionInBoundsCount++;
		}
	}

	// Output answer.
	printf("Output: %zu\n", intersectionInBoundsCount);
	return 0;
}