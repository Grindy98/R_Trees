#pragma once
#include <cstdint>
#include <string>
#include <fstream>
#include <set>
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
	shared_ptr<Node> readRoot();
	void overwriteNode(const Node& node, Offset off);
	Offset appendNewNode(const Node& node);
	
	const Header& getHeader() const;
	Offset getRootOffset() const;

	set<string> getTags() const;
	void insertTagIfUnique(string tag);
	bool doesTagExist(string tag) const;
private:
	Header header;
	fstream indexStream;
	Offset indexFileSize;
	fstream tagStream;
	set<string> tagArray;
};
