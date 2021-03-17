#pragma once
#include <cstdint>
#include <string>
#include <fstream>
#include "Offset.h"
using namespace std;

constexpr auto FILE_BUFFER_SIZE = 8192;

class IndexFile
{
public:
	struct Header {
		uint32_t keySize;
		uint32_t entrySize;
		uint32_t degree;
	};

	IndexFile(string existingFile);
	IndexFile(string fileToCreate, int keySize, int entrySize, int degree);
	IndexFile(string fileToCreate, int keySize, int entrySize);
	~IndexFile();

	void read(char bytes[], int size, Offset off);
	void write(char bytes[], int size, Offset off);

	int getEntryByteSize() const;
	Offset getEOFOffset() const;
	Offset getStartOffset() const;
	const Header& getHeader() const;
private:
	Header header;
	fstream stream;
	Offset fileSize;
};

