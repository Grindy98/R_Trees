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
		char dataFileType;
	};

	IndexFile(string existingFile);
	IndexFile(string fileToCreate, int degree, char dataFileType);
	~IndexFile();

	shared_ptr<Node> readNode(Offset off);
	void overwriteNode(const Node& node, Offset off);
	Offset appendNewNode(const Node& node);
	
	const Header& getHeader() const;
	Offset getRootOffset() const;

	vector<string> getTags() const;
	void insertTag(string tag);
private:
	Header header;
	fstream indexStream;
	Offset indexFileSize;
	fstream tagStream;
	vector<string> tagArray;
};
