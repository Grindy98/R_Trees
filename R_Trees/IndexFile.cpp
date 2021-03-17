#include "IndexFile.h"
#include <iostream>
#include <cassert>

IndexFile::IndexFile(string existingFile)
{
	stream.open(existingFile, ios::in | ios::out | ios::binary);
	// File has to exist, else ideally throw an exception
	if (!stream.is_open()) {
		throw exception("Index file does not exist");
	}

	stream.read(reinterpret_cast<char*>(&header), sizeof(Header));

	stream.seekg(0, ios::end);
	fileSize = Offset(stream.tellg());

	if (((int64_t)(fileSize.getVal()) - sizeof(header)) % getEntryByteSize() != 0) {
		stream.close();
		throw exception("Misaligned index file with entry size " + getEntryByteSize());
	}
}

IndexFile::IndexFile(string fileToCreate, int keySize, int entrySize, int order)
	:
	header({ (uint32_t)keySize, (uint32_t)entrySize, (uint32_t)order })
{
	if (entrySize < keySize) {
		throw exception("Key size at most equal with entry size");
	}
	if (order < 1) {
		throw exception("Order must be greater or equal to 1");
	}
	// Create new file, open in trunc mode and append header
	stream.open(fileToCreate, ios::in | ios::out | ios::binary | ios::trunc);
	stream.write(reinterpret_cast<const char*>(&header), sizeof(Header));

	stream.seekg(0, ios::end);
	fileSize = Offset(stream.tellg());
}

IndexFile::IndexFile(string fileToCreate, int keySize, int entrySize)
	:
	// Choose ideal degree based on the buffer size
	IndexFile(fileToCreate, keySize, entrySize, max((FILE_BUFFER_SIZE - 26 - keySize) / (2 * keySize + 32), 1))
{}

IndexFile::~IndexFile()
{
	stream.close();
}

void IndexFile::read(char bytes[], int size, Offset off)
{
	assert(((int64_t)(fileSize.getVal()) - sizeof(header)) % getEntryByteSize() == 0);
	assert(fileSize.getVal() - off.getVal() >= getEntryByteSize());
	stream.seekg(off.getVal());
	stream.read(bytes, size);
}

void IndexFile::write(char bytes[], int size, Offset off)
{
	assert(((int64_t)(fileSize.getVal()) - sizeof(header)) % getEntryByteSize() == 0);
	stream.seekp(off.getVal());
	stream.write(bytes, size);
	stream.flush();
	// Re evaluate file size
	stream.seekg(0, ios::end);
	fileSize = Offset(stream.tellg());
}

// FILE_BUFFER_SIZE >= 4 + keySize * 2 * degree + keySize + 16 * degree + 8 + 16 * degree + 16 ==
// == 28 + keySize + degree * (2 * keySize + 32) ==> FILE_BUFFER_SIZE - 26 - keySize >= (2 * keySize + 32) * degree 
// ==> degree = (FILE_BUFFER_SIZE - 26 - keySize)/(2 * keySize + 32)
int IndexFile::getEntryByteSize() const
{
	return 4 + (header.keySize + 8) * (2 * header.degree + 1) + 8 * (2 * header.degree + 2);
}

Offset IndexFile::getEOFOffset() const
{
	return fileSize;
}

Offset IndexFile::getStartOffset() const
{
	return Offset(sizeof(Header));
}

const IndexFile::Header& IndexFile::getHeader() const
{
	return header;
}
