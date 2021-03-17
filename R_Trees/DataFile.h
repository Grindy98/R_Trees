#pragma once

#include <fstream>
#include "IndexFile.h"
#include "IndexKey.h"
using namespace std;

class DataFile
{
public:
	DataFile(string filename, int keySize, int entrySize);
	DataFile(string filename, const IndexFile& idxf);
	~DataFile();

	pair<IndexKey, BinaryStorage> getEntryAtOffset(Offset off);
	void setEntryAtOffset(BinaryStorage entry, Offset off);
	Offset addNewEntry(pair<IndexKey, BinaryStorage>entry);

	Offset getEOFOffset() const;

	const uint32_t entrySize;
	const uint32_t keySize;
private:
	vector<char> bufferedRead;
	Offset bufferPointer;
	fstream stream;
	Offset fileSize;
};