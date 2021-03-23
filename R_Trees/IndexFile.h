#pragma once
#include <cstdint>
#include <string>
#include <fstream>
#include "Node.h"
using namespace std;

class IndexFile
{

public:
	struct Header {
		uint32_t degree;
	};

	IndexFile(string existingFile);
	IndexFile(string fileToCreate, int degree);
	~IndexFile();

	shared_ptr<Node> readNode(const Node& node, Offset off);
	void writeNode(const Node& node, Offset off);

	Offset getEOFOffset() const;
	Offset getStartOffset() const;
	const Header& getHeader() const;

	vector<string> getTags() const;
	void insertTag(string tag);
private:
	Header header;
	fstream indexStream;
	Offset indexFileSize;
	fstream tagStream;
	vector<string> tagArray;
};
