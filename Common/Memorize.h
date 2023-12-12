#pragma once
#include "Common.h"

// Macro setup.
#ifndef MEMORY_BUFFERSIZE
#	define MEMORY_BUFFERSIZE 1000
#endif

#ifndef MEMORY_VALUETYPE
#	define MEMORY_VALUETYPE
#endif

// Struct setup.
struct MemoryState {
	struct MemoryKey m_key;
	MEMORY_VALUETYPE MemoryValue m_value;
};
struct Memory {
	struct MemoryState m_states[MEMORY_BUFFERSIZE];
	size_t m_stateCount;
};

// Memory functions.
void Memory_reset(struct Memory* const this) {
	// Reset count.
	this->m_stateCount = 0;
}
const MEMORY_VALUETYPE MemoryValue* const Memory_add(struct Memory* const this, const struct MemoryKey key, const MEMORY_VALUETYPE MemoryValue value) {
	// Check if buffer exceeded.
	if (this->m_stateCount >= MEMORY_BUFFERSIZE) {
		fprintf(stderr, "Memory Buffer of %zu exceeded.", (size_t)MEMORY_BUFFERSIZE);
		abort();
	}

	// Add state.
	memcpy(&this->m_states[this->m_stateCount].m_key, &key, sizeof(key));
	memcpy(&this->m_states[this->m_stateCount].m_value, &value, sizeof(value));

	// Update counter.
	this->m_stateCount++;

	// Return data.
	return &this->m_states[this->m_stateCount - 1].m_value;
}
const MEMORY_VALUETYPE MemoryValue* const Memory_find(const struct Memory* const this, const struct MemoryKey key) {
	// Find memory state from key.
	for (size_t i = 0; i < this->m_stateCount; i++) {
		if (memcmp(&this->m_states[i].m_key, &key, sizeof(struct MemoryKey)) != 0) continue;
		return &this->m_states[i].m_value;
	}

	// State not found.
	return nullptr;
}