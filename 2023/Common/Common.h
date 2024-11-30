#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#define nullptr NULL

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