#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#define nullptr NULL

const size_t GCD(const size_t a, const size_t b) {
	// Exit cases.
	if (a == 0) return b;
	if (b == 0) return a;
	if (a == b) return a;

	// Light Recursion.
	if (a > b) {
		const size_t mod = a % b;
		if (mod == 0) return GCD(b, b);
		return GCD(mod, b);
	}
	const size_t mod = b % a;
	if (mod == 0) return GCD(a, a);
	return GCD(a, mod);
}
const size_t LCM(const size_t a, const size_t b) {
	return (a * b) / GCD(a, b);
}