#include "IndexFile.h"
#include <cassert>

IndexFile::IndexFile(string existingFile)
{
	// Open and read from idx file the header and open the existing tag file
	indexStream.open(existingFile + ".idx", ios::in | ios::out | ios::binary);
	tagStream.open(existingFile + ".idxt", ios::in | ios::out | ios::app);

	if (!indexStream.is_open() || !tagStream.is_open()) {
		throw exception("Index files not found!\n");
	}

	indexStream.seekp(0, ios::end);
	indexFileSize = Offset(indexStream.tellp());
	assert(indexFileSize.get() >= sizeof(Header));

	// Read header
	indexStream.read((char*)&header, sizeof(Header));

	// Read tags
	while (!tagStream.eof()) {
		string tag;
		getline(tagStream, tag);
		tagArray.push_back(tag);
	}
}

IndexFile::IndexFile(string fileToCreate, int degree)
	:
	header{(uint32_t)degree}
{
	// Open and read from idx file the header and open the existing tag file
	indexStream.open(fileToCreate + ".idx", ios::in | ios::out | ios::binary | ios::trunc);
	tagStream.open(fileToCreate + ".idxt", ios::in | ios::out | ios::app | ios::trunc);

	if (!indexStream.is_open() || !tagStream.is_open()) {
		throw exception("Error creating index files!\n");
	}
	indexStream.write((char*)&header, sizeof(Header));
	indexFileSize = getStartOffset();
}

IndexFile::~IndexFile()
{
	indexStream.close();

	tagStream.seekg(0, ios::beg);
	unsigned i = 0;
	while (!tagStream.eof()) {
		string tag;
		getline(tagStream, tag);
		assert(tagArray[i] == tag);
		i++;
	}
	for (; i < tagArray.size(); i++) {
		// Append new tags
		tagStream >> tagArray[i];
	}
	tagStream.close();
}

shared_ptr<Node> IndexFile::readNode(const Node& node, Offset off)
{
	// Check for offset alignment
	assert(off < indexFileSize);
	unsigned size = Node::byteSize(header.degree);
	assert((off.get() - sizeof(Header)) % size  == 0);

	vector<char> buf(size);
	indexStream.seekg(off.get(), ios::beg);
	indexStream.read(buf.data(), size);
	return Node::deserialize(buf, header.degree);
}

void IndexFile::writeNode(const Node& node, Offset off)
{
	// Assert alignment and position
	assert(off <= indexFileSize);
	unsigned size = Node::byteSize(header.degree);
	assert((off.get() - sizeof(Header)) % size == 0);
	if (off == indexFileSize) {
		// When writing past eof
		indexFileSize = Offset(indexFileSize + size);
	}

	indexStream.seekp(off.get(), ios::beg);
	vector<char> buf = Node::serialize(node);
	indexStream.write(buf.data(), size);
}

Offset IndexFile::getEOFOffset() const
{
	return Offset();
}

Offset IndexFile::getStartOffset() const
{
	return Offset(sizeof(Header));
}

const IndexFile::Header& IndexFile::getHeader() const
{
	return header;
}

vector<string> IndexFile::getTags() const
{
	return tagArray;
}

void IndexFile::insertTag(string tag)
{
	tagArray.push_back(tag);
	for (string x : tagArray) {
		assert(tag != x);
	}
}
