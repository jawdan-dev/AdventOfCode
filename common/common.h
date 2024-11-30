#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define nullptr (void*)NULL

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(*arr))
#define min(a, b) (a <= b ? a : b)
#define max(a, b) (a >= b ? a : b)

#define _STRINGIFY(x) #x
#define STRINGIFY(x) _STRINGIFY(x)
#define ASSERT(condition, msg) \
	if (!(condition)) { \
		printf(__FILE__ ":" STRINGIFY(__LINE__) ": " msg "\n"); \
		exit(1); \
	}

const int abs(const int v) {
	return v >= 0 ? v : -v;
}

const size_t gcd(const size_t a, const size_t b) {
	// Exit cases.
	if (a == 0) return b;
	if (b == 0) return a;
	if (a == b) return a;

	// Light Recursion.
	if (a > b) {
		const size_t mod = a % b;
		if (mod == 0) return gcd(b, b);
		return gcd(mod, b);
	}
	const size_t mod = b % a;
	if (mod == 0) return gcd(a, a);
	return gcd(a, mod);
}
const size_t lcm(const size_t a, const size_t b) {
	return (a * b) / gcd(a, b);
}

const bool expandMemory(void** memoryBlock, const size_t expectedLength, size_t* const capacity, const size_t stride) {
	ASSERT(memoryBlock != nullptr, "Invalid memory block pointer passed to expand memory.");
	ASSERT(capacity != nullptr, "Invalid capacity pointer passed to expand memory.");

	if (*memoryBlock != nullptr && *capacity >= expectedLength)
		return false;

	if (*capacity <= 0) *capacity = 1;
	while (*capacity < expectedLength)
		*capacity *= 2;

	void* const temp = realloc(*memoryBlock, stride * *capacity);
	ASSERT(temp != nullptr, "Failed to allocate expand memory block.");
	*memoryBlock = temp;

	return true;
}

#include "./fileReader.h"