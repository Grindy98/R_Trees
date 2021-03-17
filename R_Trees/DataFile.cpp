#include "DataFile.h"
#include <iostream>
#include <cassert>

DataFile::DataFile(string filename, int keySize, int entrySize)
	:
	entrySize(entrySize),
	keySize(keySize),
	bufferPointer(),
	bufferedRead(max(FILE_BUFFER_SIZE, entrySize))
{
	if (entrySize < keySize) {
		throw exception("Key size at most equal with entry size");
	}

	stream.open(filename, fstream::in | fstream::out | fstream::binary);
	if (!stream.is_open()) {
		throw exception("Data file not found");
	}

	stream.seekg(0, fstream::end);
	fileSize = Offset(stream.tellg());

	if (fileSize.getVal() % entrySize) {
		stream.close();
		throw exception("Misaligned data file with entry size " + entrySize);
	}

	// Set pointer to eof initially
	bufferPointer = fileSize;
}

DataFile::DataFile(string filename, const IndexFile& idxf)
	:
	DataFile(filename, idxf.getHeader().keySize, idxf.getHeader().entrySize)
{}

DataFile::~DataFile()
{
	stream.close();
}

pair<IndexKey, BinaryStorage> DataFile::getEntryAtOffset(Offset off)
{
	assert((off + entrySize).getVal() <= fileSize.getVal());
	assert(off.getVal() % entrySize == 0);
	// Check if buffer cannot provide this info
	if ( !(off.getVal() >= bufferPointer.getVal() && 
		(off + entrySize).getVal() <= (bufferPointer + bufferedRead.size()).getVal()) ) {
		stream.seekg(off.getVal());
		bufferPointer = off;
		// Check for eof scenario
		if ((fileSize.getVal() - off.getVal() < bufferedRead.size())) {
			stream.read(bufferedRead.data(), fileSize.getVal() - off.getVal());
		}
		else {
			stream.read(bufferedRead.data(), bufferedRead.size());		
		}
	}
	auto alignedBuffer = reinterpret_cast<unsigned char*>(bufferedRead.data() + (off.getVal() - bufferPointer.getVal()));
	IndexKey key(alignedBuffer, keySize);
	BinaryStorage entry(alignedBuffer + keySize, entrySize - keySize);
	return make_pair(key, entry);
}

void DataFile::setEntryAtOffset(BinaryStorage entry, Offset off)
{
	assert(entry.getSize() == entrySize - keySize);
	assert((off + entrySize).getVal() <= fileSize.getVal());
	assert(off.getVal() % entrySize == 0);
	stream.seekp((off + keySize).getVal());
	stream.write((const char*)(entry.bytes.data()), (uint64_t)entrySize - keySize);
	stream.flush();
	// Reset buffer
	bufferPointer = fileSize;
}

Offset DataFile::addNewEntry(pair<IndexKey, BinaryStorage> entry)
{
	assert(entry.first.getSize() == keySize);
	assert(entry.second.getSize() == entrySize - keySize);
	Offset off = fileSize;
	stream.seekp(off.getVal());
	stream.write((const char*)(entry.first.bytes.data()), keySize);
	stream.write((const char*)(entry.second.bytes.data()), (uint64_t)entrySize - keySize);
	stream.flush();
	fileSize = off + entrySize;
	// Reset buffer
	bufferPointer = fileSize;
	return off;
}

Offset DataFile::getEOFOffset() const
{
	return fileSize;
}