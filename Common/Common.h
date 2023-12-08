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

	// Values for calculation.
	size_t ra = a, rb = b;

	// Light Recursion.
	if (ra > rb) {
		while (ra > rb) ra -= rb;
		return GCD(ra, rb);
	}
	while (ra < rb) rb -= ra;
	return GCD(ra, rb);
}
const size_t LCM(const size_t a, const size_t b) {
	return (a * b) / GCD(a, b);
}