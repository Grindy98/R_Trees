#include "IndexFile.h"
#include <cassert>
#include <sstream>

IndexFile::IndexFile(string existingFile)
{
	// Open and read from idx file the header and open the existing tag file
	indexStream.open(existingFile + ".idx", ios::in | ios::out | ios::binary);
	tagStream.open(existingFile + ".idxt", ios::in | ios::out);

	if (!indexStream.is_open() || !tagStream.is_open()) {
		throw exception("Index files not found!\n");
	}

	indexStream.seekp(0, ios::end);
	indexFileSize = Offset(indexStream.tellp());
	assert(indexFileSize.get() >= sizeof(Header));

	// Read header
	indexStream.seekg(0, ios::beg);
	indexStream.read((char*)&header, sizeof(Header));
	nodeSize = Node::byteSize(header.degree);

	// Read root and store it in memory, send it to file in the destructor
	vector<char> buf(nodeSize);
	indexStream.seekg(sizeof(Header), ios::beg);
	indexStream.read(buf.data(), nodeSize);
	root =  Node::deserialize(buf, header.degree);

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
	header{(uint32_t)degree, dataFileType},
	nodeSize(Node::byteSize(degree))
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
	// Write header
	indexStream.write((char*)&header, sizeof(Header));
	
	// Write empty root for an empty tree -- only for padding
	root = make_shared<Node>(degree, true);
	appendNewNode(*root);

	indexFileSize = Offset(sizeof(Header) + nodeSize);

}

IndexFile::~IndexFile()
{
	// Write root to file
	indexStream.seekp(sizeof(Header), ios::beg);
	vector<char> buf = Node::serialize(*root);
	indexStream.write(buf.data(), nodeSize);
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
	assert(off.get() > sizeof(Header));
	assert(off < indexFileSize);
	assert((off.get() - sizeof(Header)) % nodeSize  == 0);

	vector<char> buf(nodeSize);
	indexStream.seekg(off.get(), ios::beg);
	indexStream.read(buf.data(), nodeSize);
	return Node::deserialize(buf, header.degree);
}

shared_ptr<Node> IndexFile::getRoot()
{
	return root;
}

void IndexFile::setRoot(shared_ptr<Node> newRoot)
{
	root = newRoot;
}

void IndexFile::overwriteNode(const Node& node, Offset off)
{
	// Assert alignment and position
	assert(off.get() > sizeof(Header));
	assert(off < indexFileSize);
	assert((off.get() - sizeof(Header)) % nodeSize == 0);

	indexStream.seekp(off.get(), ios::beg);
	vector<char> buf = Node::serialize(node);
	indexStream.write(buf.data(), nodeSize);
}

Offset IndexFile::appendNewNode(const Node& node)
{
	indexStream.seekp(0, ios::end);
	vector<char> buf = Node::serialize(node);
	indexStream.write(buf.data(), nodeSize);
	Offset ret = indexFileSize;
	indexFileSize = Offset(indexFileSize + nodeSize);
	return ret;
}

const IndexFile::Header& IndexFile::getHeader() const
{
	return header;
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
