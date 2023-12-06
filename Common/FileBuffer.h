#pragma once
#include "Common.h"

// Macro setup.
#ifndef FILEBUFFER_READBUFFERSIZE
#	define FILEBUFFER_READBUFFERSIZE 2000
#endif

// Struct setup.
struct FileDelim {
	char m_character;
	enum ReadState m_state;
};
struct FileBuffer {
	// Internal information.
	FILE* m_file;
	size_t m_readIndex;
	bool m_eof;
	// Delims.
	const struct FileDelim* m_delims;
	size_t m_delimCount;
	enum ReadState m_eofDelim;
	// User fun.
	char readBuffer[FILEBUFFER_READBUFFERSIZE];
	enum ReadState readState;
};


// File buffer functions.
void FileBuffer_OpenFile(
	struct FileBuffer* this, const char* filePath
) {
	// Open file.
	this->m_file = fopen(filePath, "rb");
	if (this->m_file == NULL) {
		fprintf(stderr, "Failed to open file.");
		abort();
	}

	// Initialize FileBuffer variables.
	this->m_eof = false;
	this->m_readIndex = 0;
	// Initialize delims.
	this->m_delims = nullptr;
	this->m_delimCount = 0;
	this->m_eofDelim = 0;
}
void FileBuffer_SetDelims(
	struct FileBuffer* this,
	const struct FileDelim* const delims, const size_t delimCount,
	const enum ReadState eofDelim
) {
	// Update details.
	this->m_delims = delimCount > 0 ? delims : nullptr;
	this->m_delimCount = delims != nullptr ? delimCount : 0;
	this->m_eofDelim = eofDelim;
}
const bool FileBuffer_ReadUntilNextDelim(
	struct FileBuffer* this
) {
	// If file has ended, exit.
	if (this->m_eof)
		return false;

	// Reset details.
	this->m_readIndex = 0;

	// Read file until delim.
	while (!this->m_eof) {
		if (feof(this->m_file)) {
			// Handle end of file.
			this->m_eof = true;
			this->readState = this->m_eofDelim;

			// Close the file.
			if (this->m_file != nullptr) {
				fclose(this->m_file);
				this->m_file = nullptr;
			}
			break;
		}

		// Read character.
		fread(&this->readBuffer[this->m_readIndex], sizeof(*this->readBuffer), 1, this->m_file);
		const char c = this->readBuffer[this->m_readIndex];

		// Handle delims.
		bool delimHit = false;
		for (size_t i = 0; i < this->m_delimCount; i++) {
			// Check if delim matches.
			if (c != this->m_delims[i].m_character) continue;

			// Update read state.
			this->readState = this->m_delims[i].m_state;

			// Update details.
			delimHit = true;
			break;
		}
		// Break out if delim hit.
		if (delimHit) break;


		// Normal character handling.
		if (this->m_readIndex > 0 || c != ' ')
			// Increment index.
			this->m_readIndex++;

		// Handle if buffer exceeded.
		if (this->m_readIndex > FILEBUFFER_READBUFFERSIZE) {
			fprintf(stderr, "FileBuffer ReadBuffer size exceeded.\n");
			fclose(this->m_file);
			abort();
		}
	}

	// Mark end of line.
	this->readBuffer[this->m_readIndex] = '\0';

	// Return successful read.
	return true;
}