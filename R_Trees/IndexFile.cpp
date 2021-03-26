#include "IndexFile.h"
#include <cassert>
#include <sstream>

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
	indexStream.seekg(0, ios::beg);
	indexStream.read((char*)&header, sizeof(Header));

	// Read tags
	while (tagStream) {
		string tag;
		getline(tagStream, tag);
		tagArray.insert(tag);
	}
	tagStream.clear();
}

IndexFile::IndexFile(string fileToCreate, int degree, char dataFileType)
	:
	header{(uint32_t)degree, dataFileType}
{
	// Open and read from idx file the header and open the existing tag file
	indexStream.open(fileToCreate + ".idx", ios::in | ios::out | ios::binary | ios::trunc);
	tagStream.open(fileToCreate + ".idxt", ios::in | ios::out | ios::trunc);

	if (!indexStream.is_open() || !tagStream.is_open()) {
		stringstream ss;
		ss << "Error creating index files(Index file: " << indexStream.is_open() <<
			", TagFile: " << tagStream.is_open() << ")" << endl;
		throw exception(ss.str().c_str());
	}
	indexStream.write((char*)&header, sizeof(Header));
	indexFileSize = Offset(sizeof(Header));
}

IndexFile::~IndexFile()
{
	indexStream.close();

	// Set tags to file
	tagStream.seekp(0, ios::beg);
	for (string tag : tagArray) {
		tag += "\n";
		tagStream << tag;
	}
	tagStream.close();
}

shared_ptr<Node> IndexFile::readNode(Offset off)
{
	// Check for offset alignment
	assert(off.notNull());
	assert(off < indexFileSize);
	unsigned size = Node::byteSize(header.degree);
	assert((off.get() - sizeof(Header)) % size  == 0);

	vector<char> buf(size);
	indexStream.seekg(off.get(), ios::beg);
	indexStream.read(buf.data(), size);
	return Node::deserialize(buf, header.degree);
}

shared_ptr<Node> IndexFile::readRoot()
{
	if (sizeof(Header) >= indexFileSize.get()) {
		return nullptr;
	}
	
	unsigned size = Node::byteSize(header.degree);
	vector<char> buf(size);
	indexStream.seekg(sizeof(Header), ios::beg);
	indexStream.read(buf.data(), size);
	return Node::deserialize(buf, header.degree);
}

void IndexFile::overwriteNode(const Node& node, Offset off)
{
	// Assert alignment and position
	assert(off < indexFileSize);
	unsigned size = Node::byteSize(header.degree);
	assert((off.get() - sizeof(Header)) % size == 0);

	indexStream.seekp(off.get(), ios::beg);
	vector<char> buf = Node::serialize(node);
	indexStream.write(buf.data(), size);
}

Offset IndexFile::appendNewNode(const Node& node)
{
	unsigned size = Node::byteSize(header.degree);

	indexStream.seekp(0, ios::end);
	vector<char> buf = Node::serialize(node);
	indexStream.write(buf.data(), size);
	Offset ret = indexFileSize;
	indexFileSize = Offset(indexFileSize + size);
	return ret;
}

const IndexFile::Header& IndexFile::getHeader() const
{
	return header;
}

Offset IndexFile::getRootOffset() const
{
	return Offset(sizeof(Header));
}

set<string> IndexFile::getTags() const
{
	return tagArray;
}

void IndexFile::insertTagIfUnique(string tag)
{
	tagArray.insert(tag);
}

bool IndexFile::doesTagExist(string tag) const
{
	return tagArray.find(tag) != tagArray.end();
}
